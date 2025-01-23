#include "motor_controller.h"
#include "timer_pwm.h"
#include "error.h"

/**
  @brief Setup the MotorController.
 */
void MotorController_Init(void)
{
  TimerPwm_SetDutyCycle(0u);
}

/**
  @brief Start or update started motor.
 */
void MotorController_Run(const Motor_Control_t arg_csMtrOrder)
{
  switch(arg_csMtrOrder.eDirection)
  {
    case MTR_DIR_CCW:
      DirectionController_GoCCW();
      break;

    case MTR_DIR_CW:
    default:
      // Go CW by default
      DirectionController_GoCW();
      break;
  }
  TimerPwm_SetDutyCycle(arg_csMtrOrder.u32Speed);
}

/**
  @brief Stop motor.
 */
void MotorController_Stop(void)
{
  TimerPwm_SetDutyCycle(0u);
}