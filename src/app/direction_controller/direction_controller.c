#include "direction_controller.h"

#include "board.h"
#include "stm32l5xx_ll_gpio.h"

/**
  @brief Setup DirectionController.
 */
void DirectionController_Init(void)
{
  // GPIO is instanced in system.c

  LL_GPIO_ResetOutputPin(BOARD_GPIO_MTR_DIR_PORT, BOARD_GPIO_MTR_DIR_PIN);
}

/**
  @brief Go to direction Clock Wise
 */
void DirectionController_GoCW(void)
{
  LL_GPIO_SetOutputPin(BOARD_GPIO_MTR_DIR_PORT, BOARD_GPIO_MTR_DIR_PIN);
}

/**
  @brief Go to direction Counter Clock Wise
 */
void DirectionController_GoCCW(void)
{
  LL_GPIO_ResetOutputPin(BOARD_GPIO_MTR_DIR_PORT, BOARD_GPIO_MTR_DIR_PIN);
}