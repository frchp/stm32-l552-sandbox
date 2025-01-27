#ifndef _SUPERVISOR_TASK_H_
#define _SUPERVISOR_TASK_H_

#include "FreeRTOS.h"
#include "task.h"

/* Motor task configuration */
#define SUPERVISOR_TASK_PRIORITY     (2u)
#define SUPERVISOR_TASK_STACK_SIZE   (128u)

extern TaskHandle_t gbl_sSupervisorTaskHandle;
extern StackType_t gbl_sStackSupervisor[SUPERVISOR_TASK_STACK_SIZE];
extern StaticTask_t gbl_sTCBSupervisor;

/* Events */
#define SUPERVISOR_EVT_MTRALIVE         (1u << 0u)
#define SUPERVISOR_EVT_TIMERELAPSED     (1u << 1u)

void SupervisorTask(void *arg_pvParameters);

/**
 * @brief Is called by Motor task to notify it is still alive.
 */
void SupervisorTask_MotorTaskIsAlive(void);

#endif // _SUPERVISOR_TASK_H_
