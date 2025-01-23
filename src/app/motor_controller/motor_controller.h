#ifndef _MOTOR_CONTROLLER_H_
#define _MOTOR_CONTROLLER_H_

#include <stdint.h>
#include "direction_controller.h"

typedef struct
{
  MotorDirection_t eDirection;
  uint32_t u32Speed;
} __attribute__((packed)) Motor_Control_t;

/**
  @brief Setup the MotorController.
 */
void MotorController_Init(void);

/**
  @brief Configure motor.
 */
void MotorController_Configure(Motor_Control_t arg_sMtrOrder);

/**
  @brief Start or update started motor.
 */
void MotorController_Run(void);

/**
  @brief Stop motor.
 */
void MotorController_Stop(void);

#endif // _MOTOR_CONTROLLER_H_