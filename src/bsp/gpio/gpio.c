#include <stdbool.h>
#include <stdint.h>

#include "gpio.h"

#include "board.h"

#include "stm32l5xx_ll_gpio.h"
#include "stm32l5xx_ll_bus.h"

/**
  @brief Setup the GPIO defined in configuration.
 */
void Gpio_Init(void)
{
  static bool loc_bInitialized = false;
  if(!loc_bInitialized)
  {
    loc_bInitialized = true;

    /** ADC1 GPIO Configuration for BOARD_ADC_MTR_CURRENT */
    LL_AHB2_GRP1_EnableClock(LL_AHB2_GRP1_PERIPH_GPIOC);
    LL_GPIO_SetPinMode(BOARD_ADC_MTR_CURRENT_PORT, BOARD_ADC_MTR_CURRENT_PIN, LL_GPIO_MODE_ANALOG);
    LL_GPIO_SetPinPull(BOARD_ADC_MTR_CURRENT_PORT, BOARD_ADC_MTR_CURRENT_PIN, LL_GPIO_PULL_NO);

    /** TIM1_CH1 GPIO Configuration for BOARD_GPIO_MTR_FB */
    LL_AHB2_GRP1_EnableClock(LL_AHB2_GRP1_PERIPH_GPIOA);
    LL_GPIO_SetPinMode(BOARD_GPIO_MTR_FB_PORT, BOARD_GPIO_MTR_FB_PIN, LL_GPIO_MODE_ALTERNATE);
    LL_GPIO_SetPinPull(BOARD_GPIO_MTR_FB_PORT, BOARD_GPIO_MTR_FB_PIN, LL_GPIO_PULL_UP);
    LL_GPIO_SetPinSpeed(BOARD_GPIO_MTR_FB_PORT, BOARD_GPIO_MTR_FB_PIN, LL_GPIO_SPEED_FREQ_HIGH);
    LL_GPIO_SetAFPin_0_7(BOARD_GPIO_MTR_FB_PORT, BOARD_GPIO_MTR_FB_PIN, LL_GPIO_AF_1);

    /** TIM3_CH2 GPIO Configuration for BOARD_GPIO_MTR_CTRL */
    LL_GPIO_SetPinMode(BOARD_GPIO_MTR_CTRL_PORT, BOARD_GPIO_MTR_CTRL_PIN, LL_GPIO_MODE_ALTERNATE);
    LL_GPIO_SetPinPull(BOARD_GPIO_MTR_CTRL_PORT, BOARD_GPIO_MTR_CTRL_PIN, LL_GPIO_PULL_DOWN);
    LL_GPIO_SetPinSpeed(BOARD_GPIO_MTR_CTRL_PORT, BOARD_GPIO_MTR_CTRL_PIN, LL_GPIO_SPEED_FREQ_HIGH);
    LL_GPIO_SetAFPin_0_7(BOARD_GPIO_MTR_CTRL_PORT, BOARD_GPIO_MTR_CTRL_PIN, LL_GPIO_AF_2);

    /* Direction GPIO */
    LL_GPIO_SetPinMode(BOARD_GPIO_MTR_DIR_PORT, BOARD_GPIO_MTR_DIR_PIN, LL_GPIO_MODE_OUTPUT);
    LL_GPIO_SetPinOutputType(BOARD_GPIO_MTR_DIR_PORT, BOARD_GPIO_MTR_DIR_PIN, LL_GPIO_OUTPUT_PUSHPULL);
    LL_GPIO_SetPinSpeed(BOARD_GPIO_MTR_DIR_PORT, BOARD_GPIO_MTR_DIR_PIN, LL_GPIO_SPEED_FREQ_LOW);

    /* Alive LED GPIO */
    LL_GPIO_SetPinMode(BOARD_GPIO_LED_ALIVE_PORT, BOARD_GPIO_LED_ALIVE_PIN, LL_GPIO_MODE_OUTPUT);
    LL_GPIO_SetPinOutputType(BOARD_GPIO_LED_ALIVE_PORT, BOARD_GPIO_LED_ALIVE_PIN, LL_GPIO_OUTPUT_PUSHPULL);
    LL_GPIO_SetPinSpeed(BOARD_GPIO_LED_ALIVE_PORT, BOARD_GPIO_LED_ALIVE_PIN, LL_GPIO_SPEED_FREQ_LOW);
  }
}

/**
  @brief Set given GPIO to given state (true = HIGH, false = LOW).
 */
void Gpio_Set(GPIO_TypeDef *arg_GpioReg, uint32_t arg_u32Pin, bool arg_bLevel)
{
  if(arg_bLevel)
  {
    LL_GPIO_SetOutputPin(arg_GpioReg, arg_u32Pin);
  }
  else
  {
    LL_GPIO_ResetOutputPin(arg_GpioReg, arg_u32Pin);
  }
}

/**
  @brief Toggle given GPIO.
 */
void Gpio_Toggle(GPIO_TypeDef *arg_GpioReg, uint32_t arg_u32Pin)
{
  LL_GPIO_TogglePin(arg_GpioReg, arg_u32Pin);
}
