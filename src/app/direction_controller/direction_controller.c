#include "direction_controller.h"

#include "board.h"
#include "gpio.h"

static MotorDirection_t gbl_eCurrentDirection;

/**
  @brief Setup DirectionController.
 */
void DirectionController_Init(void)
{
  // GPIO is instanced in system.c

  Gpio_Set(BOARD_GPIO_MTR_DIR_PORT, BOARD_GPIO_MTR_DIR_PIN, false);
  gbl_eCurrentDirection = MTR_DIR_CCW;
}

/**
  @brief Go to direction Clock Wise
 */
void DirectionController_GoCW(void)
{
  Gpio_Set(BOARD_GPIO_MTR_DIR_PORT, BOARD_GPIO_MTR_DIR_PIN, true);
  gbl_eCurrentDirection = MTR_DIR_CW;
}

/**
  @brief Go to direction Counter Clock Wise
 */
void DirectionController_GoCCW(void)
{
  Gpio_Set(BOARD_GPIO_MTR_DIR_PORT, BOARD_GPIO_MTR_DIR_PIN, false);
  gbl_eCurrentDirection = MTR_DIR_CCW;
}