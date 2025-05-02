#if PROJECT_F103

#include "../../include/application/application.h"
#include "../../../../include/application/project/stm32f103/appStm32F103.h"
#include "../../include/hal/hal.h"//TODO/move to service.h

AppStm32F103 *AppStm32F103::_this = nullptr;

int AppStm32F103::close(){
     MutexLock ML(&_objMutex);
     return ActiveObject::close();
}

int AppStm32F103::open(void * arg){
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
          _appTimer = xTimerCreate("appTimer", TIMER_INTERVAL, pdTRUE, (void *)0, _timerHandler);
          if(_appTimer != NULL){
               xTimerStart(_appTimer, 0);
          }
#endif
          _objState = objStateOpened;
     }
     LOG("[mkOS] Version: " OS_VERSION "/ Author: " OS_AUTHOR "/ Date: " __DATE__ "/ Time: " __TIME__);
     LOG("[Project] Name: " PROJECT_NAME "/ Version: " PROJECT_VERSION "/ Author: " PROJECT_AUTHOR);
     _threadProcedure(_this);
     return 0;
}

int AppStm32F103::sync(int32_t sync, void* arg1, void* arg2, void* arg3, void* arg4){
     if(_objState < objStateOpened){
          LOG("AppStm32F103 is not opened");
          return -1;
     }
     MutexLock ML(&_objMutex);
     switch(sync){
          default:
               break;
     }
     return ActiveObject::sync(sync, arg1, arg2, arg3, arg4);
}

void AppStm32F103::_threadProcedure(void* _this){
     if(_this){
          ((AppStm32F103 *)_this)->_threadHandler();
     }
}

void AppStm32F103::_threadHandler(){
     if(_objState < objStateOpened){
          LOG("AppStm32F103 is not opened");
          return;
     }
     request _request;
     for(;;){
          if(_activeObjectSema.take()){
               MutexLock ML(&_objMutex);
               while(_requestIsrQueue.totalRequestSize > (int)sizeof(_request)){
                    _requestIsrQueue.pull((uint8_t *)&_request, sizeof(_request));
                    if(_request.payloadSize){
                         _requestIsrQueue.pull(_requestPayload, _request.payloadSize);
                    }
                    _requestHandler(&_request, _requestPayload);
               }
               while(_requestQueue.totalRequestSize > (int)sizeof(_request)){
                    _requestQueue.pull((uint8_t *)&_request, sizeof(_request));
                    if(_request.payloadSize){
                         _requestQueue.pull(_requestPayload, _request.payloadSize);
                    }
                    _requestHandler(&_request, _requestPayload);
               }
          }
     }
}

void AppStm32F103::_requestHandler(request* request, uint8_t* payload){
     switch(request->sync){
          case requestAppTimer:{
               Led::get()->sync(Led::ledTimer);
               break;
          }
     }
}

void AppStm32F103::_timerHandler(TimerHandle_t xTimer){
     AppStm32F103::get()->sync(syncRequest, (void*)requestAppTimer);
}

#endif