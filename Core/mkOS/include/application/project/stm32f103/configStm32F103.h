#pragma once

#ifdef __cplusplus
extern "C" {
#endif

#ifdef PROJECT_F103
// =============================================================================
// [PROJECT SYSTEM CONFIGURATION]
// =============================================================================
#define PROJECT_NAME        "PROJECT_F103"
#define PROJECT_VERSION     "0.0.1"
#define PROJECT_AUTHOR      "mkkim"

#define PROJECT_MCU         STM32F103C8T6
#define PROJECT_RTOS        FREERTOS
#define PROJECT_SDK         STM32

#define PROJECT_LOG_ENABLE  1
#if PROJECT_LOG_ENABLE
    #define LOG LOG_UART
#endif

#define PROJECT_TIMER_ENABLE    1
#if PROJECT_TIMER_ENABLE
    #define TIMER_INTERVAL  10
#endif

#define PROJECT_RXBUFFER_SIZE   256 
#define PROJECT_CLI_ENABLE  1

// =============================================================================
// [PROJECT RTOS CONFIGURATION]
// =============================================================================
#ifdef PROJECT_RTOS
    #define THREAD_STACK_SIZE           512 // byte
    #define REQUEST_MAX_COUNT           4         
    #define REQUEST_PAYLOAD_MAX_SIZE    256
    #define REQUEST_BUFFER_SIZE         REQUEST_MAX_COUNT * REQUEST_PAYLOAD_MAX_SIZE    
#endif

// =============================================================================
// [PERIPHERAL CONFIGURATION]
// =============================================================================

// USB
#define USB_DEVICE USB_XXX
#if USB_DEVICE == USB_CDC
    #define USB_CDC_RX_QUEUE_SIZE   258
#endif

// UART
#define UART UART_INTERNAL
#if UART
    #define UART_MAX_CHANNEL    1
#endif

// DMA
#define DMA_ENABLE          1
#if DMA_ENABLE && UART
    #define DMA_UART        1
#endif

// =============================================================================
// [SENSOR / MODULE CONFIGURATION]
// =============================================================================

// LED
#define LED LED_PWM


#endif// PROJECT_F103

#ifdef __cplusplus
}
#endif