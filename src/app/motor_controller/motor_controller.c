#include "motor_controller.h"
#include "timer_pwm.h"

/**
  @brief Setup the MotorController.
 */
void MotorController_Init(void)
{
  TimerPwm_SetDutyCycle(0u);
}

/**
  @brief Start or update motor.
 */
void MotorController_Run(uint32_t arg_u32Speed, MotorDirection_t arg_eDirection)
{
  switch(arg_eDirection)
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
  TimerPwm_SetDutyCycle(arg_u32Speed);
}

/**
  @brief Stop motor.
 */
void MotorController_Stop(void)
{
  TimerPwm_SetDutyCycle(0u);
}