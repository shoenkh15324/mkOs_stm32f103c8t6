#pragma once
#include "../../system/system.h"

#if PROJECT_F103
#include "configStm32F103.h"

class AppStm32F103 : public ActiveObject{
protected:
     static AppStm32F103 *_this;
     alignas(4) uint8_t _requestPayload[REQUEST_PAYLOAD_MAX_SIZE];
public:
     enum{
          requestAppTimer = 0,
     };
public:
     static inline AppStm32F103* get(){
          if(_this){
               return _this;
          }
          _this = new AppStm32F103;
          return _this;
     }
     int close();
     int open(void * = nullptr);
     int sync(int32_t, void* = nullptr, void* = nullptr, void* = nullptr, void* = nullptr);
     static void _threadProcedure(void *);
protected:
     TimerHandle_t _appTimer;
     void _threadHandler();
     void _requestHandler(request*, uint8_t*);
     static void _timerHandler(TimerHandle_t);
};
#endif