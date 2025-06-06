#include "../../include/hal/hal.h"
/*
     Stm32CubeMx Setting: 
          - TIM3 (CH1, PWM Generation CH1)
          - Prescaler: 1800 - 1
          - Counter Period: 400 - 1
          - Pin: PA6
*/
#if LED == LED_PWM
#include "tim.h"

Led Led::_instance;

int Led::close(){
     if(_objState >= objStateOpened){
          _objState = objStateClosing;
          MutexLock ML(&_objMutex);
          HAL_TIM_PWM_Stop(&htim3, TIM_CHANNEL_1);
          //
          _objState = objStateClosed;
     }
     return 0;
}

int Led::open(void* arg){
     MutexLock ML(&_objMutex);
     close();
     HAL_TIM_PWM_Start(&htim3, TIM_CHANNEL_1);
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
                         _dutyCycle = 0;
                         _repeatCnt = 0;
                         break;
                    case ledStatePowerOn:{
                         _ledFading(&htim3, TIM_CHANNEL_1, 1, 1500, 1000, 1000, 500);
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

void Led::_ledFading(TIM_HandleTypeDef *htim, uint32_t ch, int8_t repeatCnt, uint16_t risingTime, uint16_t onTime, uint16_t fallingTime, uint16_t offTime) {
     uint32_t now = Oal::getTick();
     uint32_t elapsed = now - _ledTick;
     uint32_t totalCycle = risingTime + onTime + fallingTime + offTime;

     if (_repeatCnt >= repeatCnt && repeatCnt != -1) {
          _objState = objStateOpened;
          return;
     }
     if (elapsed < risingTime) {
          _dutyCycle = (LED_DUTYCYCLE_MAX * elapsed) / risingTime;
     } else if (elapsed < risingTime + onTime) {
          _dutyCycle = LED_DUTYCYCLE_MAX;
     } else if (elapsed < risingTime + onTime + fallingTime) {
          uint32_t t = elapsed - (risingTime + onTime);
          _dutyCycle = LED_DUTYCYCLE_MAX - (LED_DUTYCYCLE_MAX * t) / fallingTime;
     } else if (elapsed < totalCycle) {
          _dutyCycle = 0;
     } else {
          _ledTick = now;
          _dutyCycle = 0;
          if (repeatCnt != -1) ++_repeatCnt;
     }
     __HAL_TIM_SET_COMPARE(htim, ch, _dutyCycle);
}

void Led::_ledBlinking(TIM_HandleTypeDef *htim, uint32_t ch, int8_t repeatCnt, uint16_t onTime, uint16_t offTime) {
     uint32_t now = Oal::getTick();
     uint32_t elapsed = now - _ledTick;

     if (_repeatCnt >= repeatCnt && repeatCnt != -1) {
          _objState = objStateOpened;
          return;
     }
     if (elapsed < onTime) {
          __HAL_TIM_SET_COMPARE(htim, ch, LED_DUTYCYCLE_MAX);
     } else if (elapsed < onTime + offTime) {
          __HAL_TIM_SET_COMPARE(htim, ch, 0);
     } else {
          _ledTick = now;
          if (repeatCnt != -1) {
               ++_repeatCnt;
          }
     }
}
#endif