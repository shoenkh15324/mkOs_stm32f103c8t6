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
     if(_objState < objStateOpened){
          LOG("Uart is not opened");
          return -1;
     }
     MutexLock ML(&_objMutex);
     switch (sync){
          case uartSend:{
               UART_HandleTypeDef *channel = (UART_HandleTypeDef *)arg1;
               uint8_t *data = (uint8_t *)arg2;
               uint16_t size = (uint16_t)(intptr_t)arg3;
               if(HAL_OK != HAL_UART_Transmit(channel, data, size, HAL_MAX_DELAY)){
                    LOG("uartSent Fail");
                    return -1;
               }
               break;
          }
          case uartReceive:{
               UART_HandleTypeDef *channel = (UART_HandleTypeDef *)arg1;
               uint8_t *data = (uint8_t *)arg2;
               uint16_t size = (uint16_t)(intptr_t)arg3;
               if(HAL_OK != HAL_UART_Receive(channel, data, size, HAL_MAX_DELAY)){
                    LOG("uartReceive Fail");
                    return -1;
               }
               break;
          }
#if DMA_UART
          case uartSendDma:{
               UART_HandleTypeDef *channel = (UART_HandleTypeDef *)arg1;
               uint8_t *data = (uint8_t *)arg2;
               uint16_t size = (uint16_t)(intptr_t)arg3;
               if(HAL_OK != HAL_UART_Transmit_DMA(channel, data, size)){
                    LOG("uartSendDma Fail");
                    return -1;
               }
               break;
          }
          case uartReceiveDma:{
               UART_HandleTypeDef *channel = (UART_HandleTypeDef *)arg1;
               uint8_t *data = (uint8_t *)arg2;
               uint16_t size = (uint16_t)(intptr_t)arg3;
               if(HAL_OK != HAL_UART_Receive_DMA(channel, data, size)){
                    LOG("uartReceiveDma Fail");
                    return -1;
               }
               break;
          }
#endif
          default:
               LOG("Unregisterd sync");
               break;
     }
     return 0;
}
#endif