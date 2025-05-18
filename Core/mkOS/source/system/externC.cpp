#include "../../include/hal/hal.h"
#include "../../include/system/externC.h"
#include "../../include/application/project/stm32f103/application.h"

void* systemSync(int32_t sync , void* arg1, void* arg2, void* arg3, void* arg4){
     switch(sync){
#if USB_DEVICE == USB_CDC
          case sysSyncUsbCdcRxCallback:{
               Application::get()->sync(Application::asyncPayload, (void*)Application::appUsbCdcRxCallback, arg1, arg2);
               break;
          }
#endif
          default:
               break;
     }
     return nullptr;
}