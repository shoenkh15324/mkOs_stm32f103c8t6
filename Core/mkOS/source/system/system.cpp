#include "../../include/application/project/stm32f103/application.h"

//Object
int Object::getObjState(){
     return _objState;
}

// Mutex Lock
MutexLock::MutexLock(OalMutex *mutex){
     if(_mutex && mutex->lock()){
          _mutex = mutex;
     }
}

MutexLock::~MutexLock(){
     if(_mutex){
          _mutex->unlock();
     }
}

// ActiveObject
ActiveObject::ActiveObject() : _activeObjectSema(-1, 0){}
     
int ActiveObject::close(){
     //MutexLock ML(&_objMutex);
     return 0;
}

int ActiveObject::open(void *arg){
     if(!arg){
          LOG("arg == NULL");
          return -1;
     }
     MutexLock ML(&_objMutex);
     _threadMetadata = *(threadMetadata *)arg;
     _requestQueue.open(_threadMetadata.requestBufferSize);
     _requestIsrQueue.open(_threadMetadata.requestIsrBufferSize);
     return 0;
}

int ActiveObject::sync(int32_t sync, void *arg1, void *arg2, void *arg3, void *arg4){
     if(_objState < objStateOpened) { 
          LOG("ActiveObject is not opened");
          return -1;
     }
     const bool inIsr = Oal::isInIsr();
     RequestQueue &queue = inIsr ? _requestIsrQueue : _requestQueue;

     switch (sync) {
          case syncRequest: {
               request req = {(unsigned)arg1, arg2, arg3, arg4, 0};
               if (queue.push(reinterpret_cast<uint8_t*>(&req), sizeof(req)) != sizeof(req)) {
                    LOG("requestQueue.push fail");
                    return -1;
               }
               _activeObjectSema.give();
               break;
          }
          case syncRequestPayload: {
               int payloadSize = reinterpret_cast<intptr_t>(arg3);
               if (payloadSize > _threadMetadata.requestPayloadMaxSize) {
                    LOG("%d > requestPayloadMaxSize(%d)", payloadSize, _threadMetadata.requestPayloadMaxSize);
                    return -1;
               }
               request req = {(unsigned)arg1, arg4, nullptr, nullptr, (uint16_t)payloadSize};
               if (queue.canPush() < ((int)sizeof(req) + (int)payloadSize)) {
                    LOG("requestQueue.canPush() < (request:%d + payload:%d)", sizeof(req), payloadSize);
                    return -1;
               }
               queue.push(reinterpret_cast<uint8_t*>(&req), sizeof(req));
               queue.push(reinterpret_cast<uint8_t*>(arg2), payloadSize);
               _activeObjectSema.give();
               break;
          }
          case syncRequestExpress: {
               _activeObjectSema.give();
               break;
          }
          default: {
               LOG("Unregistered Sync: %d", sync);
               return -1;
          }
     }
     return 0;
}

int mkOsOpen(){
#if dgCfgSdkOs == egCfgSdkOsFreeRtos
     Application* app = Application::get();
     xTaskCreate(Application::_applicationOpen, "mainThread", THREAD_STACK_SIZE, app, 1, NULL);
     vTaskStartScheduler();
     LOG("Should not reach here if scheduler started.");
     for (;;);
     return 0;
#endif
}