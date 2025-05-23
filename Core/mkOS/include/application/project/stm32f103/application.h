#pragma once
#include "../../system/system.h"

#if PROJECT_F103
#include "configStm32F103.h"

class Application : public ActiveObject{
     friend int mkOsOpen();
     friend int ActiveObject::sync(int32_t, void*, void*, void*, void*);

public:
     enum{
          appTimer = 0,
     };
     
private:
     static Application _instance;

protected:
     Application() = default; // 생성자는 private로 외부에서 인스턴스를 생성하지 못하도록 제한

public:
     static inline Application *get() { return &_instance; }
     Application(const Application&) = delete;  // 복사 생성자 삭제
     Application& operator=(const Application&) = delete;  // 복사 대입 연산자 삭제
     int close();
     int open(void * = nullptr);
     int sync(int32_t, void* = nullptr, void* = nullptr, void* = nullptr, void* = nullptr);
     
protected:
     TimerHandle_t _appTimer;
     alignas(4) uint8_t _messagePayload[MESSAGE_PAYLOAD_MAX_SIZE];
     void _popMessage();
     void _threadHandler();
     void _messageHandler(message*, uint8_t*);
     static void _timerHandler(TimerHandle_t);
     static void _threadProcedure(void *);
     static void _applicationOpen(void *);
};
#endif