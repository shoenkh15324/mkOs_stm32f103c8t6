#pragma once//TODO/Need Optimization
#if USB_DEVICE == USB_CDC

class Usb : public Object{
public:
     enum{// USB CDC Sync
          usbCdcSend = Object::syncBegin,
          usbCdcReceive,//TODO/Need Implement
     };
protected:
     static Usb *_this;
public:
     static inline Usb* get(){
          if(_this){
               return _this;
          }
          _this = new Usb;
          return _this;
     }
public:
     int close();
     int open(void * = nullptr);
     int sync(int32_t, void * = nullptr, void * = nullptr, void * = nullptr, void * = nullptr);
};

#endif