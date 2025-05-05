#pragma once

#ifdef __cplusplus
extern "C" {
#endif
// =============================================================================
// [OS INFO]
// =============================================================================
#define OS_NAME            "mkOs"
#define OS_VERSION         "1.0.1"
#define OS_AUTHOR          "Minkyu Kim"

// =============================================================================
// [SYSTEM CONFIGURATION]
// =============================================================================
#define MKOS_ENABLE     1

#define LOG_PRINTF      1
#define LOG_UART        2

// =============================================================================
// [MCU CONFIGURATION]
// =============================================================================
#define STM32F103C8T6   1
#define STM32F103RB     2
#define STM32F411CEU6   3

// =============================================================================
// [RTOS CONFIGURATION]
// =============================================================================
#define NO_RTOS         1
#define FREERTOS        2

// =============================================================================
// [SDK CONFIGURATION]
// =============================================================================
#define STM32           1
#define ESP_IDF         2
#define NRF_CONNECT     3

// =============================================================================
// [PERIPHERAL CONFIGURATION]
// =============================================================================

// UART
#define UART_INTERNAL       1
#define UART_XXX            2

// USB
#define USB_CDC             1
#define USB_ADC             2
#define USB_DFU             3
#define USB_HID             4
#define USB_CUSTOM_HID      5
#define USB_MSC             6
#define USB_XXX             7

// I2C
#define I2C_INTERNAL        1
#define I2C_XXX             2


// =============================================================================
// [SENSOR & MODULE CONFIGURATION]
// =============================================================================

// LED
#define LED_INTERNAL        1
#define LED_PWM             2
#define LED_XXX             3


// =============================================================================
// [PROJECT CONFIGURATION]
// =============================================================================
#define PROJECT_F103    1
#ifdef PROJECT_F103
    #include "../application/project/stm32f103/configStm32F103.h"
#endif


#ifdef __cplusplus
}
#endif