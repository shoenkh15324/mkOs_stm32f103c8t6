#pragma once

#include "../system/system.h"

class Oal{
public:
     static int open(void *arg1);
     static int isInIsr();
     static uint32_t getTick();
     static void taskDelay(int delay);
     static void mcuDelay(int delay);
};

class OalSemaphore{
protected:
#if PROJECT_RTOS == FREERTOS
     SemaphoreHandle_t _semaphore = nullptr;
#endif
public:
     OalSemaphore(uint16_t maxCnt = -1, uint16_t initialCnt = 0);
     ~OalSemaphore();
     bool take(int ms = -1);
     void give();
};

class OalMutex{
protected:
#if PROJECT_RTOS == FREERTOS
     SemaphoreHandle_t _mutex = nullptr;
#endif
     int _mutexLocked = 0;
public:
     OalMutex();
     ~OalMutex();
     bool lock(int ms = -1);
     void unlock();
};