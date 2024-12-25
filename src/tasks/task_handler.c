#include "task_handler.h"

#include "FreeRTOS.h"
#include "task.h"
#include "FreeRTOSConfig.h"

#include "media_task.h"
#include "motor_task.h"

/**
  @brief Setup OS tasks.
 */
void TaskHandler_Init(void)
{
  // Create MEDIA Task
  MediaTaskHandle = xTaskCreateStatic(MediaTask,            // Task function
              "MEDIA",              // Task name
              MEDIA_TASK_STACK_SIZE,                  // Stack size in words
              NULL,                 // Task parameter
              tskIDLE_PRIORITY + MEDIA_TASK_PRIORITY, // Task priority
              gbl_sStackMedia,
              &gbl_sTCBMedia );

  // Create MOTOR_DRIVING Task
  MotorDrivingTaskHandle = xTaskCreateStatic(MotorDrivingTask,            // Task function
              "MOTOR_DRIVING",             // Task name
              MOTOR_TASK_STACK_SIZE,                         // Stack size in words
              NULL,                        // Task parameter
              tskIDLE_PRIORITY + MOTOR_TASK_PRIORITY,        // Task priority
              gbl_sStackMotor,
              &gbl_sTCBMotor );
}

/**
  @brief Start OS.
 */
void TaskHandler_StartOS(void)
{
  // Start the FreeRTOS Scheduler
  vTaskStartScheduler();
}
