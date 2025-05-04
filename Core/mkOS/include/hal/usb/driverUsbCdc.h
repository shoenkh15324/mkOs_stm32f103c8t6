#pragma once//TODO/Need Optimization
#if USB_DEVICE == USB_CDC

class Usb : public Object{
public:
     enum{// USB CDC Sync
          usbCdcSend = Object::syncBegin,
          usbCdcReceive,//TODO/Need Implement
          usbCdcRxCallback,
     };
private:
     static Usb _instance;
     Usb();
     Usb(const Usb &) = delete;
     Usb& operator=(const Usb &) = delete;
public:
     static inline Usb *get() { return &_instance; }
protected:
     uint8_t _usbCdcRxBuf[USB_CDC_RX_QUEUE_SIZE] = {0};
     uint8_t _head = 0, _tail = 0;
public:
     int close();
     int open(void * = nullptr);
     int sync(int32_t, void * = nullptr, void * = nullptr, void * = nullptr, void * = nullptr);
};
#endif