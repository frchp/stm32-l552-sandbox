#ifndef _BOARD_H_
#define _BOARD_H_

#include "stm32l552xx.h"
#include "stm32l5xx_ll_adc.h"
#include "stm32l5xx_ll_gpio.h"

//! List the GPIO connexions

// TIM1_CH1
#define BOARD_GPIO_MTR_CTRL_PORT                          GPIOA
#define BOARD_GPIO_MTR_CTRL_PIN                           (LL_GPIO_PIN_8)

// TIM2_CH1
#define BOARD_GPIO_MTR_FB_PORT                            GPIOA
#define BOARD_GPIO_MTR_FB_PIN                             (LL_GPIO_PIN_0)

#define BOARD_GPIO_MTR_DIR_PORT                           GPIOA
#define BOARD_GPIO_MTR_DIR_PIN                            (LL_GPIO_PIN_1)

#define BOARD_GPIO_LED1_GPIO_PORT                         GPIOC
#define BOARD_GPIO_LED1_PIN                               (LL_GPIO_PIN_7)

#define BOARD_GPIO_LED2_GPIO_PORT                         GPIOB
#define BOARD_GPIO_LED2_PIN                               (LL_GPIO_PIN_7)

#define BOARD_GPIO_LED3_GPIO_PORT                         GPIOA
#define BOARD_GPIO_LED3_PIN                               (LL_GPIO_PIN_9)

//! List the ADC connexions
// ADC1_CH0
#define BOARD_ADC_MTR_CURRENT_PORT                        GPIOC
#define BOARD_ADC_MTR_CURRENT_PIN                         (LL_GPIO_PIN_0)
#define BOARD_ADC_MTR_CURRENT_CHANNEL                     (LL_ADC_CHANNEL_0)

#endif // _BOARD_H_