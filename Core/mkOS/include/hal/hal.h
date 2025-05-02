#pragma once

#include "../system/system.h"

#if UART == UART_INTERNAL
     #include "uart/driverUart.h"
#endif

#if LED == LED_PWM
     #include "led/driverLedPwm.h"
#elif LED == LED_INTERNAL
     #include "led/driverLedInternal.h"
#endif