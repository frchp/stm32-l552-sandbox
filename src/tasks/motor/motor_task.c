#include "motor_task.h"

/* Motor task parameters */
TaskHandle_t MotorDrivingTaskHandle;
StackType_t gbl_sStackMotor [MOTOR_TASK_STACK_SIZE];
StaticTask_t gbl_sTCBMotor;

void MotorDrivingTask(void *pvParameters)
{
    (void)pvParameters; // Avoid compiler warning for unused parameter

    for (;;)
    {
        // Block for 200 ms
        vTaskDelay(pdMS_TO_TICKS(200));
    }
}
