#include "../../include/hal/hal.h"
/*
     STM32F103C8T6: 
          - GPIO: PB12
          - GPIO Output Level: Low
          - GPIO Pull Up/Pull Down: No Pull Up and no Pull Down
*/
#if LED == LED_INTERNAL
#include "gpio.h"

Led Led::_instance;

int Led::close(){
     if(_objState >= objStateOpened){
          _objState = objStateClosing;
          MutexLock ML(&_objMutex);
          //
          _objState = objStateClosed;
     }
     return 0;
}

int Led::open(void* arg){
     MutexLock ML(&_objMutex);
     close();
     //
     _objState = objStateOpened;
     return 0;
}

int Led::sync(int32_t sync, void *arg1, void *arg2, void *arg3, void *arg4){
     if(_objState < objStateOpened){
          LOG("Led is not opened");
          return -1;
     }
     MutexLock ML(&_objMutex);
     switch (sync){
          case ledTimer:
               switch(_objState){
                    case objStateOpened:
                         _ledTick = Oal::getTick();
                         _repeatCnt = 0;
                         break;
                    case ledStatePowerOn:{
                         _ledBlinking(3, 200, 500);
                         break;
                    }
               }
               break;
          case ledPowerOn:
               _objState = ledStatePowerOn;
               break;
          case ledPowerOff:
               _objState = ledStatePowerOff;
               break;
          default:
               LOG("Unregisterd sync");
               break;
     }
     return 0;
}

void Led::_ledBlinking(int8_t repeatCnt, uint16_t onTime, uint16_t offTime) {
     uint32_t now = Oal::getTick();
     uint32_t elapsed = now - _ledTick;

     if (_repeatCnt >= repeatCnt && repeatCnt != -1) {
          HAL_GPIO_WritePin(LED_GPIO_TYPE, LED_GPIO_PIN, GPIO_PIN_SET); // LED OFF
          _objState = objStateOpened;
          return;
     }
     if (elapsed < onTime) {
          HAL_GPIO_WritePin(LED_GPIO_TYPE, LED_GPIO_PIN, GPIO_PIN_RESET); // LED ON
     } else if (elapsed < onTime + offTime) {
          HAL_GPIO_WritePin(LED_GPIO_TYPE, LED_GPIO_PIN, GPIO_PIN_SET);   // LED OFF
     } else {
          _ledTick = now;
          if (repeatCnt != -1) {
               ++_repeatCnt;
          }
     }
}
#endif