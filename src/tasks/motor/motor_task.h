#ifndef _MOTOR_TASK_H_
#define _MOTOR_TASK_H_

#include "FreeRTOS.h"
#include "task.h"

/* Motor task configuration */
#define MOTOR_TASK_PRIORITY     (2u)
#define MOTOR_TASK_STACK_SIZE   (128u)

extern TaskHandle_t gbl_sMotorDrivingTaskHandle;
extern StackType_t gbl_sStackMotor[MOTOR_TASK_STACK_SIZE];
extern StaticTask_t gbl_sTCBMotor;

/* Events */
#define MTR_EVT_GO       (1u << 0u)
#define MTR_EVT_STOP     (1u << 1u)

void MotorDrivingTask_Notify(uint32_t arg_u32Evt);

void MotorDrivingTask_Run(void *arg_vMtrCfg);

void MotorDrivingTask(void *arg_pvParameters);

#endif // _MOTOR_TASK_H_
