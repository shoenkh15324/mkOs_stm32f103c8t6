#pragma once

extern "C" {
     #include <stdlib.h>
     #include <stdio.h>
     #include <stdarg.h>
     #include <string.h>
}

#if PROJECT_RTOS == FREERTOS
extern "C" {
     #include "../../external/freeRtos/include/FreeRTOS.h"
     #include "../../external/freeRtos/include/task.h"
     #include "../../external/freeRtos/include/semphr.h"
     #include "../../external/freeRtos/portable/ARM_CM3/portmacro.h"
     #include "../../external/freeRtos/include/timers.h"
}
#endif

#if PROJECT_SDK == STM32
extern "C" {
     #include "../../../../Drivers/STM32F1xx_HAL_Driver/Inc/stm32f1xx_hal.h"
}
#endif

#include <algorithm>
#include <cstdint>
#include <memory>
#include "systemConfig.h"
#include "buffer.h"
#include "../oal/oal.h"

// LOG
#if LOG == LOG_UART
extern "C" {
     #include "stm32f1xx_hal_uart.h"
     #include "usart.h"
}    
     #define LOG_UART_INSTANCE huart1
     constexpr size_t LOG_BUFFER_SIZE = 128;
     #define LOG(fmt, ...) do{ \
          char buf[LOG_BUFFER_SIZE]; \
          int len = snprintf(buf, sizeof(buf), "[mkOS] %s/%d : " fmt "\r\n", __func__, __LINE__, ##__VA_ARGS__); \
          HAL_UART_Transmit(&LOG_UART_INSTANCE, reinterpret_cast<uint8_t*>(buf), len, HAL_MAX_DELAY); \
     } while(0)
#elif LOG == LOG_PRINTF
     #include <cstdio>
     #include <cstdarg>

     inline void log_printf(const char* format, ...) {
     std::printf("[mkOs] ");
     va_list args;
     va_start(args, format);
     std::vprintf(format, args);
     va_end(args);
     std::printf("\n");
     }

     #define LOG(fmt, ...)    log_printf(fmt, ##__VA_ARGS__)
#endif

// Object
class Object{
public:
     enum{
          objStateClosed = 0,
          objStateClosing,
          objStateOpened,
          objStateBegin,
     };

     enum{
          syncBegin = 0,
     };

protected:
     int _objState = objStateClosed;
     OalMutex _objMutex;

public:
#if 0
     virtual int close() = 0;
     virtual int open(void * = nullptr) = 0;
     virtual int sync(int32_t , void* = nullptr, void* = nullptr, void* = nullptr, void* = nullptr) = 0;
#endif
     int getObjState();
};

// Mutex Lock
class MutexLock{
protected:
     OalMutex *_mutex = nullptr;

public:
     MutexLock(OalMutex *mutex);
     ~MutexLock();
};

// ActiveObject
class ActiveObject : public Object{
public:
     enum{// ActiveObject Sync
          async = syncBegin,
          asyncPayload,
          asyncExpress,
     };

     typedef struct{
          int16_t threadStackSize;
          void (*fnProcedure)(void*);
          int16_t messageQueueSize;
          int16_t messageIsrQueueSize;
          int16_t asyncPayloadMaxSize;
     } threadMetadata;

     typedef struct{
          int16_t sync;
          void *hArg1;
          void *hArg2;
          void *hArg3;
          uint16_t payloadSize;
     } request;

protected:
     MessageQueue _messageQueue, _messageIsrQueue;
     OalSemaphore _activeObjectSema;
     threadMetadata _threadMetadata;

public:
     ActiveObject();
     int close();
     int open(void* = nullptr);
     int sync(int32_t sync, void* = nullptr, void* = nullptr, void* = nullptr, void* = nullptr);
};

#ifdef __cplusplus
extern "C"{
#endif
     int mkOsOpen();
#ifdef __cplusplus
}
#endif