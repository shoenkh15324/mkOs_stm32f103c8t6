#include "../../include/hal/hal.h"

#if USB_DEVICE == USB_CDC
#include "usbd_cdc_if.h"

Usb *Usb::_this = nullptr;

int Usb::close(){
     if(_objState >= objStateOpened){
          _objState = objStateClosing;
          MutexLock ML(&_objMutex);
          //
          _objState = objStateClosed;
     }
     return 0;
}

int Usb::open(void* arg){
     MutexLock ML(&_objMutex);
     close();
     //
     _objState = objStateOpened;
     return 0;
}

int Usb::sync(int32_t sync, void *arg1, void *arg2, void *arg3, void *arg4){
     if(_objState < objStateOpened){
          LOG("USB CDC is not Opened");
          return -1;
     }
     MutexLock ML(&_objMutex);
     switch (sync){
          case usbCdcSend:{
               uint8_t *buf = (uint8_t *)arg1;
               uint16_t len = (unsigned)arg2;
               if(USBD_OK != CDC_Transmit_FS(buf, len)){
                    return -1;
               }
               break;
          }
          default:
               LOG("Unregisterd sync");
               break;
     }
     return 0;
}
#endif