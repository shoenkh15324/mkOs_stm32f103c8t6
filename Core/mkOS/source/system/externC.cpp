#include "../../include/hal/hal.h"
#include "../../include/system/externC.h"
#include "../../include/application/project/stm32f103/application.h"

void* systemSync(int32_t sync , void* arg1, void* arg2, void* arg3, void* arg4){
     switch(sync){
          default:
               break;
     }
     return nullptr;
}

int mkOsOpen(){
     #if dgCfgSdkOs == egCfgSdkOsFreeRtos
          Application* app = Application::get();
          xTaskCreate(app->_applicationOpen, "mainThread", THREAD_STACK_SIZE, app, 1, NULL);
          vTaskStartScheduler();
          LOG("Should not reach here if scheduler started.");
          for (;;);
          return 0;
     #endif
}