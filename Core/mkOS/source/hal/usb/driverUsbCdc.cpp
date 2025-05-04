#include "../../include/hal/hal.h"

#if USB_DEVICE == USB_CDC
#include "usbd_cdc.h"
#include "usbd_cdc_if.h"
#include "usbd_cdc_if_template.h"

Usb Usb::_instance;

Usb::Usb(){}

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
          case usbCdcReceive:{
               auto buf = static_cast<uint8_t *>(arg1);
               auto bufLen = static_cast<unsigned>(reinterpret_cast<uintptr_t>(arg2));
               uint16_t bytesRead = 0;
               // 먼저: '\n'이 버퍼에 존재하는지 확인
               uint8_t i = _tail;
               bool hasLine = false;
               while (i != _head) {
                    if (_usbCdcRxBuf[i] == '\n') {
                         hasLine = true;
                         break;
                    }
                    i = (i + 1) % USB_CDC_RX_QUEUE_SIZE;
               }
               // 줄이 있어야 읽기 시작
               if (hasLine) {
                    while (bytesRead < bufLen && _tail != _head) {
                         uint8_t ch = _usbCdcRxBuf[_tail];
                         buf[bytesRead++] = ch;
                         _tail = (_tail + 1) % USB_CDC_RX_QUEUE_SIZE;
                         if (ch == '\n') break;
                    }
               }else{
                    return -1;
               }
               break;
          }
          case usbCdcRxCallback:{
               auto data = static_cast<uint8_t *>(arg1);
               auto size = static_cast<unsigned>(reinterpret_cast<uintptr_t>(arg2));
               while(size--){
                    uint8_t next = (_head + 1) % USB_CDC_RX_QUEUE_SIZE;
                    if(next == _tail){
                         LOG("_usbCdcRxBuf overflow");
                         break;
                    }
                    _usbCdcRxBuf[_head] = *data++;
                    _head = next;
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