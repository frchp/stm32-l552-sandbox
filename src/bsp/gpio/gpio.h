#ifndef _GPIO_H_
#define _GPIO_H_

#include <stdbool.h>
#include <stdint.h>
#include "stm32l5xx_ll_gpio.h"

/**
  @brief Setup the GPIO defined in configuration.
 */
void Gpio_Init(void);

/**
  @brief Set given GPIO to given state (true = HIGH, false = LOW).
 */
void Gpio_Set(GPIO_TypeDef *arg_GpioReg, uint32_t arg_u32Pin, bool arg_bLevel);

/**
  @brief Toggle given GPIO.
 */
void Gpio_Toggle(GPIO_TypeDef *arg_GpioReg, uint32_t arg_u32Pin);


#endif // _GPIO_H_