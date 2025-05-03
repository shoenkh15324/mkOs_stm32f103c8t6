#include "../../include/hal/hal.h"

#if UART == UART_INTERNAL
#include "usart.h"
#include "stm32f1xx_hal_uart.h"

Uart *Uart::_this = nullptr;

int Uart::close(){
     if(_objState >= objStateOpened){
          _objState = objStateClosing;
          MutexLock ML(&_objMutex);
          //
          _objState = objStateClosed;
     }
     return 0;
}

int Uart::open(void* arg){
     MutexLock ML(&_objMutex);
     close();
     //
     _objState = objStateOpened;
     return 0;
}
// arg1: uart ch, arg2: data, arg3: data length
int Uart::sync(int32_t sync, void *arg1, void *arg2, void *arg3, void *arg4){
     if (_objState < objStateOpened) {
          LOG("Uart is not opened");
          return -1;
     }
     MutexLock ML(&_objMutex);

     UART_HandleTypeDef *channel = static_cast<UART_HandleTypeDef *>(arg1);
     uint8_t *data = static_cast<uint8_t *>(arg2);
     uint16_t size = static_cast<uint16_t>(reinterpret_cast<intptr_t>(arg3));
     auto handleResult = [](HAL_StatusTypeDef status, const char* errMsg){
          if (status != HAL_OK) {
               LOG("%s", errMsg);
               return -1;
          }
          return 0;
     };
     switch (sync) {
          case uartSend:
               return handleResult(HAL_UART_Transmit(channel, data, size, HAL_MAX_DELAY), "uartSend Fail");
          case uartReceive:
               return handleResult(HAL_UART_Receive(channel, data, size, HAL_MAX_DELAY), "uartReceive Fail");
     #if DMA_UART
          case uartSendDma:
               return handleResult(HAL_UART_Transmit_DMA(channel, data, size), "uartSendDma Fail");
          case uartReceiveDma:
               return handleResult(HAL_UART_Receive_DMA(channel, data, size), "uartReceiveDma Fail");
     #endif
          default:
               LOG("Unregistered sync");
               return -1;
     }
}
#endif