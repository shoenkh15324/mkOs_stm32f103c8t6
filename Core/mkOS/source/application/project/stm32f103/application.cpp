#if PROJECT_F103
#include "../../../../include/application/project/stm32f103/application.h"
#include "../../include/hal/hal.h"//TODO/move to service.h
#if PROJECT_CLI_ENABLE
#include "cli.h"
#endif

Application Application::_instance;

int Application::close(){
     MutexLock ML(&_objMutex);
#if PROJECT_TIMER_ENABLE
     if(pdPASS != xTimerDelete(_appTimer, portMAX_DELAY)){
          LOG("Delete Timer Fail");
          return -1;
     }
#endif
     return ActiveObject::close();
}

int Application::open(void * arg){
     {
          MutexLock ML(&_objMutex);
          threadMetadata _mainThread = {0, _threadProcedure, MESSAGE_QUEUE_SIZE, MESSAGE_QUEUE_SIZE, MESSAGE_PAYLOAD_MAX_SIZE};
          if(ActiveObject::open(&_mainThread)){
               LOG("mkOs thread open fail");
               return -1;
          }
          Cli::get()->open();
          Led::get()->open();
          Led::get()->sync(Led::ledPowerOn);

#if PROJECT_TIMER_ENABLE
          _appTimer = xTimerCreate("appTimer", TIMER_INTERVAL, pdTRUE, nullptr, _timerHandler);
          if(_appTimer){
               xTimerStart(_appTimer, 0);
          }
#endif
          _objState = objStateOpened;
     }
     LOG("[mkOS] Version: " OS_VERSION "/ Author: " OS_AUTHOR "/ Date: " __DATE__ "/ Time: " __TIME__);
     LOG("[Project] Name: " PROJECT_NAME "/ Version: " PROJECT_VERSION "/ Author: " PROJECT_AUTHOR);
     _threadHandler();
     return 0;
}

int Application::sync(int32_t sync, void* arg1, void* arg2, void* arg3, void* arg4){
     if(_objState < objStateOpened){
          LOG("Application is not opened");
          return -1;
     }
     MutexLock ML(&_objMutex);
     switch(sync){
          default:
               break;
     }
     return ActiveObject::sync(sync, arg1, arg2, arg3, arg4);
}

void Application::_applicationOpen(void* arg){
     Application::get()->open();
}

void Application::_threadProcedure(void* arg){
     auto* app = static_cast<Application*>(arg);
     if (app) app->_threadHandler();
}

void Application::_threadHandler(){
     if(_objState < objStateOpened){
          LOG("Application is not opened");
          return;
     }
     while (true) {
          if (_activeObjectSema.take()) {
               _popMessage();
          }
     }
}

void Application::_popMessage(){
     message req;
     MutexLock lock(&_objMutex);
     auto popMessage = [&](MessageQueue& queue) {
          while (queue.totalMessageSize > static_cast<int>(sizeof(req))) {
               queue.pull(reinterpret_cast<uint8_t*>(&req), sizeof(req));
               if (req.payloadSize) {
               queue.pull(_messagePayload, req.payloadSize);
               }
               _messageHandler(&req, _messagePayload);
          }
     };
     popMessage(_messageIsrQueue);
     popMessage(_messageQueue);
}

void Application::_messageHandler(message* message, uint8_t* payload){
     switch(message->sync){
          case appTimer:{  
               Led::get()->sync(Led::ledTimer);
               Cli::get()->sync(Cli::cliSyncMain);
               break;
          }
          // await

          default:
               break;
     }
}

void Application::_timerHandler(TimerHandle_t xTimer){
     Application::get()->sync(async, (void*)appTimer);
}

#endif