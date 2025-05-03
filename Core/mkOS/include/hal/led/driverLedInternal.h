#pragma once

#if LED == LED_INTERNAL

#define LED_GPIO_TYPE    GPIOB
#define LED_GPIO_PIN     GPIO_PIN_12

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
     Led() : _ledTick(0), _repeatCnt(0) {}
     Led(const Led&) = delete; 
     Led& operator=(const Led&) = delete;
public:
     static inline Led* get() { return &_instance; }
private:
     uint32_t _ledTick;
     uint8_t _repeatCnt;
public:
     int close();
     int open(void * = nullptr);
     int sync(int32_t, void * = nullptr, void * = nullptr, void * = nullptr, void * = nullptr);
     void _ledBlinking(int8_t, uint16_t, uint16_t);
};
#endif