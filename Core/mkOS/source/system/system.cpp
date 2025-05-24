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
     _messageQueue.open(_threadMetadata.messageQueueSize);
     _messageIsrQueue.open(_threadMetadata.messageIsrQueueSize);
     return 0;
}

int ActiveObject::sync(int32_t sync, void *arg1, void *arg2, void *arg3, void *arg4){
     if(_objState < objStateOpened) { 
          LOG("ActiveObject is not opened");
          return -1;
     }
     const bool inIsr = Oal::isInIsr();
     MessageQueue &queue = inIsr ? _messageIsrQueue : _messageQueue;

     switch (sync) {
          case async: {
               message msg = {(unsigned)arg1, arg2, arg3, arg4, 0};
               if (queue.push(reinterpret_cast<uint8_t*>(&msg), sizeof(msg)) != sizeof(msg)) {
                    LOG("messageQueue.push fail");
                    return -1;
               }
               _activeObjectSema.give();
               break;
          }
          case asyncPayload: {
               int payloadSize = reinterpret_cast<intptr_t>(arg3);
               if (payloadSize > _threadMetadata.asyncPayloadMaxSize) {
                    LOG("%d > asyncPayloadMaxSize(%d)", payloadSize, _threadMetadata.asyncPayloadMaxSize);
                    return -1;
               }
               message msg = {(unsigned)arg1, arg4, nullptr, nullptr, (uint16_t)payloadSize};
               if (queue.canPush() < ((int)sizeof(msg) + (int)payloadSize)) {
                    LOG("messageQueue.canPush() < (message:%d + payload:%d)", sizeof(msg), payloadSize);
                    return -1;
               }
               queue.push(reinterpret_cast<uint8_t*>(&msg), sizeof(msg));
               queue.push(reinterpret_cast<uint8_t*>(arg2), payloadSize);
               _activeObjectSema.give();
               break;
          }
          case asyncAwait:{
               message msg = {(unsigned)arg1, arg2, arg3, arg4, 0};
               if (queue.push(reinterpret_cast<uint8_t*>(&msg), sizeof(msg)) != sizeof(msg)) {
                    LOG("messageQueue.push fail");
                    return -1;
               }
               _activeObjectSema.give();

               while(_response.completed){
                    Application::get()->_popMessage();
               }
               return _response.result;
          }
          case asyncPayloadAwait:{
               int payloadSize = reinterpret_cast<intptr_t>(arg3);
               if (payloadSize > _threadMetadata.asyncPayloadMaxSize) {
                    LOG("%d > asyncPayloadMaxSize(%d)", payloadSize, _threadMetadata.asyncPayloadMaxSize);
                    return -1;
               }
               message msg = {(unsigned)arg1, arg4, nullptr, nullptr, (uint16_t)payloadSize};
               if (queue.canPush() < ((int)sizeof(msg) + (int)payloadSize)) {
                    LOG("messageQueue.canPush() < (message:%d + payload:%d)", sizeof(msg), payloadSize);
                    return -1;
               }
               queue.push(reinterpret_cast<uint8_t*>(&msg), sizeof(msg));
               queue.push(reinterpret_cast<uint8_t*>(arg2), payloadSize);
               _activeObjectSema.give();

               while(_response.completed){
                    Application::get()->_popMessage();
               }
               return _response.result;
          }
          case asyncExpress:
               _activeObjectSema.give();
               break;
          default:
               LOG("Unregistered Sync: %d", sync);
               return -1;
     }
     return 0;
}

