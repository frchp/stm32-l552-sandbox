#include "motor_controller.h"
#include "timer_pwm.h"
#include "error.h"

static Motor_Control_t gbl_MtrStatus = {MTR_DIR_CW, 0u};

/**
  @brief Setup the MotorController.
 */
void MotorController_Init(void)
{
  TimerPwm_SetDutyCycle(0u);
}

/**
  @brief Configure motor.
 */
void MotorController_Configure(Motor_Control_t arg_sMtrOrder)
{
  switch(arg_sMtrOrder.eDirection)
  {
    case MTR_DIR_CW:
    case MTR_DIR_CCW:
      gbl_MtrStatus.eDirection = arg_sMtrOrder.eDirection;
      break;

    default:
      Error_Handler();
      break;
  }
  gbl_MtrStatus.u32Speed = arg_sMtrOrder.u32Speed;
}

/**
  @brief Start or update started motor.
 */
void MotorController_Run(void)
{
  switch(gbl_MtrStatus.eDirection)
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
  TimerPwm_SetDutyCycle(gbl_MtrStatus.u32Speed);
}

/**
  @brief Stop motor.
 */
void MotorController_Stop(void)
{
  TimerPwm_SetDutyCycle(0u);
}