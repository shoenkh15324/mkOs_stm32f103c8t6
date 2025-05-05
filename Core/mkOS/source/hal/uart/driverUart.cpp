#include "../../include/hal/hal.h"

#if UART == UART_INTERNAL
#include "usart.h"
#include "stm32f1xx_hal_uart.h"

Uart Uart::_instance;
extern UART_HandleTypeDef huart1;
extern DMA_HandleTypeDef hdma_usart1_rx;

Uart::Uart(){}

int Uart::close(){
     if(_objState >= objStateOpened){
          _objState = objStateClosing;
          MutexLock ML(&_objMutex);
          //
#if DMA_UART
          _rxBuf.close();
#endif
          _objState = objStateClosed;
     }
     return 0;
}

int Uart::open(void* arg){
     MutexLock ML(&_objMutex);
     close();
     //
#if DMA_UART
     _rxBuf.open();
     if(HAL_UART_Receive_DMA(&huart1, (uint8_t*)_rxBuf.rxBuf, PROJECT_RXBUFFER_SIZE) != HAL_OK){
          return -1;
     }
     _rxBuf.head = PROJECT_RXBUFFER_SIZE - hdma_usart1_rx.Instance->CNDTR;
     _rxBuf.tail = _rxBuf.head;
#endif
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
     UART_HandleTypeDef *uartChannel = _uartChannelConfig(arg1);
     switch (sync) {
          case uartSend:{
               uint8_t *data = static_cast<uint8_t *>(arg2);
               uint16_t size = static_cast<uint16_t>(reinterpret_cast<intptr_t>(arg3));
               if(HAL_UART_Transmit(uartChannel, data, size, HAL_MAX_DELAY) != HAL_OK){
                    LOG("uartSend Fail");
                    return -1;
               }
               break;
          }
          case uartReceive:{
               uint8_t *data = static_cast<uint8_t *>(arg2);
               uint16_t size = static_cast<uint16_t>(reinterpret_cast<intptr_t>(arg3));
               if(HAL_UART_Receive(uartChannel, data, size, 10) != HAL_OK){
                    LOG("uartReceive Fail");
                    return -1;
               }
               break;
          }
#if DMA_UART
          case uartAvailableDma:{
               _rxBuf.head = PROJECT_RXBUFFER_SIZE - hdma_usart1_rx.Instance->CNDTR;
               return _rxBuf.available();
          }
          case uartReadDma:{
               uint8_t *buf = static_cast<uint8_t *>(arg1);
               uint16_t len = static_cast<uint16_t>(reinterpret_cast<intptr_t>(arg2));
               _rxBuf.read(buf, len);
               break;
          }
#endif
          default:
               LOG("Unregistered sync");
               break;
     }
     return 0;
}

UART_HandleTypeDef * Uart::_uartChannelConfig(void* arg){
     UART_HandleTypeDef *ret = nullptr;
     switch(static_cast<uint16_t>(reinterpret_cast<intptr_t>(arg))){
          case CH1:
               ret = &huart1;
               break;
#if UART_MAX_CHANNEL >= 2
          case CH2:
               ret = &huart2;
               break;
#endif
#if UART_MAX_CHANNEL >= 3
          case CH3:
               ret = &huart3;
               break;
#endif
#if UART_MAX_CHANNEL >= 4
          case CH4:
               ret = &huart4;
               break;
#endif
     }
     return ret;
}
#endif