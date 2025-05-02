#pragma once

#if LED == LED_PWM

#define LED_DUTYCYCLE_MAX (400 - 1)

class Led : public Object{
public:
     enum{// Led Sync
          ledTimer = syncBegin,
          ledPowerOn,
          ledPowerOff,
     };
     enum{// Led State
          ledStatePowerOn = objStateBegin,
          ledStatePowerOff,
     };

protected:
     static Led *_this;
public:
     static inline Led* get(){
          if(_this){
               return _this;
          }
          _this = new Led;
          return _this;
     }
protected:
     uint32_t _ledTick = 0;
     uint8_t _repeatCnt = 0;
     uint16_t _dutyCycle = 0;

public:
     int close();
     int open(void * = nullptr);
     int sync(int32_t, void * = nullptr, void * = nullptr, void * = nullptr, void * = nullptr);
     void _ledFading(TIM_HandleTypeDef *, uint32_t, int8_t, uint16_t, uint16_t, uint16_t, uint16_t);
     void _ledBlinking(TIM_HandleTypeDef *, uint32_t, int8_t, uint16_t, uint16_t);
};
#endif