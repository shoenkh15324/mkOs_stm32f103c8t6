#pragma once
#if UART == UART_INTERNAL

class Uart : public Object{
public:
     enum{
          CH1 = 0,
          CH2,
          CH3,
          CH4,
     };
     enum{// Uart Sync
          uartSend = Object::syncBegin,
          uartReceive,
#if DMA_UART
          uartWriteDma,
          uartReadDma,
#endif
     };
private:
     static Uart _instance;
     Uart();
     Uart(const Uart &) = delete;
     Uart& operator=(const Uart &) = delete;
public:
     static inline Uart *get() { return &_instance; }
private:
     RxBuffer _rxBuf;
     UART_HandleTypeDef *_uartChannelConfig(void *arg);
public:
     int close();
     int open(void * = nullptr);
     int sync(int32_t, void * = nullptr, void * = nullptr, void * = nullptr, void * = nullptr);
};
#endif