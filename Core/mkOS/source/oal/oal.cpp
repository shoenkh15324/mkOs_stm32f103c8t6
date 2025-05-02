#include "../../include/system/system.h"

// Oal
int Oal::open(void *arg){
    return 0;
}
int Oal::isInIsr(){
#if PROJECT_RTOS == FREERTOS
    return xPortIsInsideInterrupt();
#else
    return 0;
#endif
}
uint32_t Oal::getTick(){
#if PROJECT_RTOS == FREERTOS
    return (uint32_t)(xTaskGetTickCount() * portTICK_PERIOD_MS);
#else
    return 0;
#endif 
}
void Oal::taskDelay(int delay){
#if PROJECT_RTOS == FREERTOS
    vTaskDelay(pdMS_TO_TICKS(delay));
#endif
}
void Oal::mcuDelay(int delay){
    HAL_Delay(delay);
}

// OalSemaphore
OalSemaphore::OalSemaphore(uint16_t maxCnt, uint16_t initialCnt){
#if PROJECT_RTOS == FREERTOS
    _semaphore = xSemaphoreCreateCounting(maxCnt, initialCnt);
    if(!_semaphore){ LOG("_semaphore creation fail"); }
#endif
}
OalSemaphore::~OalSemaphore(){
#if PROJECT_RTOS == FREERTOS
    if(_semaphore){ vSemaphoreDelete(_semaphore); }
#endif
}
bool OalSemaphore::take(int ms){
#if PROJECT_RTOS == FREERTOS
    if(!_semaphore){ LOG("_semaphore == null");
        return false;
    }
    return pdTRUE == Oal::isInIsr() ? xSemaphoreTakeFromISR(_semaphore, nullptr) : xSemaphoreTake(_semaphore, ms < 0 ? portMAX_DELAY : pdMS_TO_TICKS(ms));
#else
    return false;
#endif
}
void OalSemaphore::give(){
#if PROJECT_RTOS == FREERTOS
    if(!_semaphore){ LOG("_semaphore == null");
        return;
    }
    if(Oal::isInIsr()){
        if(xSemaphoreGiveFromISR(_semaphore, nullptr) != pdPASS) { LOG("xSemaphoreGiveFromISR() != pdPASS"); }
    }else{
        if(xSemaphoreGive(_semaphore) != pdPASS) { LOG("xSemaphoreGive() != pdPASS"); }
    }
#endif
}

// OalMutex
OalMutex::OalMutex(){
#if PROJECT_RTOS == FREERTOS
    _mutex = xSemaphoreCreateRecursiveMutex();
    if(!_mutex){ LOG("_mutex creation fail"); }
#endif
}
OalMutex::~OalMutex(){
#if PROJECT_RTOS == FREERTOS
    if(_mutex){ vSemaphoreDelete(_mutex); }
#endif
}
bool OalMutex::lock(int ms){
    if(Oal::isInIsr()){
        return false;
    }
#if PROJECT_RTOS == FREERTOS
    if(!_mutex){ LOG("_psMutexSema == null");
        return false;
    }
    if(pdTRUE == xSemaphoreTakeRecursive(_mutex, ms < 0 ? portMAX_DELAY : pdMS_TO_TICKS(ms))){
        _mutexLocked++;
        return true;
    }
#endif
    LOG("Mutex Lock Fail");
    return false;
}
void OalMutex::unlock(){
    if(Oal::isInIsr()){
        return;
    }
    if(_mutexLocked <= 0){ LOG("Mutex Not Locked(%d)", _mutexLocked);
        return;
    }
#if PROJECT_RTOS == FREERTOS
    if(!_mutex){ LOG("_psMutexSema == null");
        return;
    }
    xSemaphoreGiveRecursive(_mutex);
#endif
    _mutexLocked--;
}
