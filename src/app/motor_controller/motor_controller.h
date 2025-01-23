#ifndef _MOTOR_CONTROLLER_H_
#define _MOTOR_CONTROLLER_H_

#include <stdint.h>
#include "direction_controller.h"

typedef struct
{
  uint8_t au8Padding[3u];
  MotorDirection_t eDirection;
  uint32_t u32Speed;
} __attribute__((packed)) Motor_Control_t;

/**
  @brief Setup the MotorController.
 */
void MotorController_Init(void);

/**
  @brief Start or update started motor.
 */
void MotorController_Run(Motor_Control_t arg_sMtrOrder);

/**
  @brief Stop motor.
 */
void MotorController_Stop(void);

#endif // _MOTOR_CONTROLLER_H_