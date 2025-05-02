#include "../../include/application/application.h"
#include "../../include/application/project/stm32f103/appStm32F103.h"

int Application::close(){
     AppStm32F103::get()->close();
     return 0;
}

void Application::open(void *arg){
     if(AppStm32F103::get()->open(arg)){
          LOG("AppStm32F103 Thread Open Fail");
          return;
     }
     return;
}

int Application::sync(int32_t sync, void* arg1, void* arg2, void* arg3, void* arg4){
     switch (sync){
          case threadAppStm32F103:
               AppStm32F103::get()->sync(sync, arg1, arg2, arg3, arg4);
               break;
     }
     return 0;
}

void Application::threadOpen(void* arg){
     Application *self = (Application *)(arg);
     self->open(arg);
}