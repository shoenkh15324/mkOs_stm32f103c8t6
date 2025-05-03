#pragma once

#include "../system/system.h"

// =============================================================================
// [PERIPHERAL]
// =============================================================================

#if UART == UART_INTERNAL
     #include "uart/driverUart.h"
#endif

#if USB_DEVICE == USB_CDC
     #include "usb/driverUsbCdc.h"
#endif

// =============================================================================
// [SENSOR / MODULE]
// =============================================================================

#if LED == LED_PWM
     #include "led/driverLedPwm.h"
#elif LED == LED_INTERNAL
     #include "led/driverLedInternal.h"
#endif