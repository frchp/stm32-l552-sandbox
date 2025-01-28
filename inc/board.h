#ifndef _BOARD_H_
#define _BOARD_H_

#include "stm32l552xx.h"
#include "stm32l5xx_ll_adc.h"

//! List the GPIO connections

// TIM3_CH2
#define BOARD_GPIO_MTR_CTRL_PORT                          GPIOC
#define BOARD_GPIO_MTR_CTRL_PIN                           (GPIO_BSRR_BS7)

// TIM1_CH1
#define BOARD_GPIO_MTR_FB_PORT                            GPIOA
#define BOARD_GPIO_MTR_FB_PIN                             (GPIO_BSRR_BS8)

#define BOARD_GPIO_MTR_DIR_PORT                           GPIOA
#define BOARD_GPIO_MTR_DIR_PIN                            (GPIO_BSRR_BS1)

// LED1
#define BOARD_GPIO_LED_ALIVE_PORT                         GPIOA
#define BOARD_GPIO_LED_ALIVE_PIN                          (GPIO_BSRR_BS9)

// UART_TX
#define BOARD_GPIO_UART_TX_PORT                           GPIOG
#define BOARD_GPIO_UART_TX_PIN                            (GPIO_BSRR_BS7)

// UART_RX
#define BOARD_GPIO_UART_RX_PORT                           GPIOG
#define BOARD_GPIO_UART_RX_PIN                            (GPIO_BSRR_BS8)

//! List the ADC connections
// ADC1_CH0
#define BOARD_ADC_MTR_CURRENT_PORT                        GPIOC
#define BOARD_ADC_MTR_CURRENT_PIN                         (GPIO_BSRR_BS0)
#define BOARD_ADC_MTR_CURRENT_CHANNEL                     (LL_ADC_CHANNEL_0)

#endif // _BOARD_H_