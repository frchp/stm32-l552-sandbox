#ifndef _MOTOR_CONTROLLER_H_
#define _MOTOR_CONTROLLER_H_

#include <stdint.h>
#include "direction_controller.h"

/**
  @brief Setup the MotorController.
 */
void MotorController_Init(void);

/**
  @brief Start or update motor.
 */
void MotorController_Run(uint32_t arg_u32Speed, MotorDirection_t arg_eDirection);



/**
  @brief Stop motor.
 */
void MotorController_Stop(void);

#endif // _MOTOR_CONTROLLER_H_