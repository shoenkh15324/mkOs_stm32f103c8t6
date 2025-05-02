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
protected:
     static Uart *_this;
public:
     static inline Uart* get(){
          if(_this){
               return _this;
          }
          _this = new Uart;
          return _this;
     }
public:
     int close();
     int open(void * = nullptr);
     int sync(int32_t, void * = nullptr, void * = nullptr, void * = nullptr, void * = nullptr);
};

#endif