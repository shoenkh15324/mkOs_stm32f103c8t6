#pragma once
#if UART == UART_INTERNAL

class Uart : public Object{
public:
     enum{// Uart Sync
          uartSend = Object::syncBegin,
          uartReceive,
#if DMA_UART
          uartSendDma,
          uartReceiveDma,
#endif
     };
private:
     static Uart _instance;
     Uart();
     Uart(const Uart &) = delete;
     Uart& operator=(const Uart &) = delete;
public:
     static inline Uart *get() { return &_instance; }
public:
     int close();
     int open(void * = nullptr);
     int sync(int32_t, void * = nullptr, void * = nullptr, void * = nullptr, void * = nullptr);
};
#endif