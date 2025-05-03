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

private:
     static Led _instance;
     Led() : _ledTick(0), _repeatCnt(0), _dutyCycle(0){}
     Led(const Led&) = delete; 
     Led& operator=(const Led&) = delete;
public:
     static inline Led* get() { return &_instance; }
private:
     uint32_t _ledTick;
     uint8_t _repeatCnt;
     uint16_t _dutyCycle;
public:
     int close();
     int open(void * = nullptr);
     int sync(int32_t, void * = nullptr, void * = nullptr, void * = nullptr, void * = nullptr);
     void _ledFading(TIM_HandleTypeDef *, uint32_t, int8_t, uint16_t, uint16_t, uint16_t, uint16_t);
     void _ledBlinking(TIM_HandleTypeDef *, uint32_t, int8_t, uint16_t, uint16_t);
};
#endif