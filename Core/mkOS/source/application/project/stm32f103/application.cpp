#if PROJECT_F103

#include "../../../../include/application/project/stm32f103/application.h"
#include "../../include/hal/hal.h"//TODO/move to service.h

Application Application::_instance;

int Application::close(){
     MutexLock ML(&_objMutex);
     return ActiveObject::close();
}

int Application::open(void * arg){
     {
          MutexLock ML(&_objMutex);
          threadMetadata _mainThread = {0, _threadProcedure, REQUEST_BUFFER_SIZE, REQUEST_BUFFER_SIZE, REQUEST_PAYLOAD_MAX_SIZE};
          if(ActiveObject::open(&_mainThread)){
               LOG("mkOs thread open fail");
               return -1;
          }
          Uart::get()->open();
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

void Application::_threadProcedure(void* arg){
     auto* app = static_cast<Application*>(arg);
     if (app) app->_threadHandler();
}

void Application::_threadHandler(){
     if(_objState < objStateOpened){
          LOG("Application is not opened");
          return;
     }
     request req;
     while (true) {
          if (_activeObjectSema.take()) {
               MutexLock lock(&_objMutex);
               auto popRequest = [&](RequestQueue& queue) {
                    while (queue.totalRequestSize > static_cast<int>(sizeof(req))) {
                         queue.pull(reinterpret_cast<uint8_t*>(&req), sizeof(req));
                         if (req.payloadSize) {
                         queue.pull(_requestPayload, req.payloadSize);
                         }
                         _requestHandler(&req, _requestPayload);
                    }
               };
               popRequest(_requestIsrQueue);
               popRequest(_requestQueue);
          }
     }
}

void Application::_requestHandler(request* request, uint8_t* payload){
     switch(request->sync){
          case appTimer:{
               Led::get()->sync(Led::ledTimer);
               uint8_t buf[] = "Hello USB";
               Usb::get()->sync(Usb::usbCdcSend, buf, (void*)(uintptr_t)sizeof(buf));
               break;
          }
     }
}

void Application::_timerHandler(TimerHandle_t xTimer){
     Application::get()->sync(syncRequest, (void*)appTimer);
}

void Application::_applicationOpen(void* arg){
     Application::get()->open();
}

#endif