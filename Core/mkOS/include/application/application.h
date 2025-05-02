#pragma once

#include "../system/system.h"

class Application{
public:
     enum{
          threadAppStm32F103 = 0,
     };
public:
     int close();
     void open(void *);
     int sync(int32_t, void* = nullptr, void* = nullptr, void* = nullptr, void* = nullptr);
     static void threadOpen(void *);
};
