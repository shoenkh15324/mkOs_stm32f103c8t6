#include "../../include/hal/hal.h"
#include "../../include/system/externC.h"
#include "../../include/application/project/stm32f103/application.h"

void* systemSync(int32_t sync , void* arg1, void* arg2, void* arg3, void* arg4){
     switch(sync){
          case sysSyncUsbCdcRxQueuePush:
               break;
          default:
               break;
     }
     return nullptr;
}