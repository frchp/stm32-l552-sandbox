#include <stdbool.h>

#include "motor_task.h"

#include "error.h"
#include "FreeRTOS.h"
#include "task.h"
#include "queue.h"

// Task specific application
#include "motor_controller.h"
#include "supervisor_task.h"

/* Motor task parameters */
TaskHandle_t gbl_sMotorDrivingTaskHandle;
StackType_t gbl_sStackMotor [MOTOR_TASK_STACK_SIZE];
StaticTask_t gbl_sTCBMotor;

/* Queue motor config */
#define MTR_QUEUE_LENGTH            5u
#define MTR_QUEUE_ITEM_SIZE         sizeof( Motor_Control_t )
static StaticQueue_t gbl_sMotorDrivingCfgQueue;
static uint8_t gbl_au8MtrCfgQueueStorage[ MTR_QUEUE_LENGTH * MTR_QUEUE_ITEM_SIZE ];
static QueueHandle_t gbl_sMotorDrivingQueueHdl;

static bool gbl_bTaskInitialized = false;

void MotorDrivingTask_Notify(uint32_t arg_u32Evt)
{
  BaseType_t xHigherPriorityTaskWoken = pdFALSE;
  if(gbl_bTaskInitialized)
  {
    if(xPortIsInsideInterrupt() != 0u)
    {
        // In interrupt context
        xTaskNotifyFromISR( gbl_sMotorDrivingTaskHandle,
                        arg_u32Evt,
                        eSetBits,
                        &xHigherPriorityTaskWoken );
        portYIELD_FROM_ISR( xHigherPriorityTaskWoken );
    }
    else
    {
        xTaskNotify( gbl_sMotorDrivingTaskHandle,
                            arg_u32Evt,
                            eSetBits );
    }
  }
}

void MotorDrivingTask_Run(void *arg_vMtrCfg)
{
  BaseType_t xHigherPriorityTaskWoken = pdFALSE;
  if(gbl_bTaskInitialized)
  {
    if(xPortIsInsideInterrupt() != 0u)
    {
      // In interrupt context
      xQueueSendToBackFromISR(gbl_sMotorDrivingQueueHdl,
                    arg_vMtrCfg,
                    &xHigherPriorityTaskWoken);
      portYIELD_FROM_ISR( xHigherPriorityTaskWoken );
    }
    else
    {
      xQueueSendToBack(gbl_sMotorDrivingQueueHdl,
                    arg_vMtrCfg,
                    0u);
    }
  }
  MotorDrivingTask_Notify(MTR_EVT_GO);
}

void MotorDrivingTask(void *arg_pvParameters)
{
  (void)arg_pvParameters; // Avoid compiler warning for unused parameter
  uint32_t u32Notifications = 0ul;
  uint32_t u32Status = pdPASS;
  Motor_Control_t sMotorCfg;

  gbl_sMotorDrivingQueueHdl = xQueueCreateStatic( MTR_QUEUE_LENGTH,
                                 MTR_QUEUE_ITEM_SIZE,
                                 gbl_au8MtrCfgQueueStorage,
                                 &gbl_sMotorDrivingCfgQueue );
  configASSERT( gbl_sMotorDrivingQueueHdl );
  gbl_bTaskInitialized = true;

  for (;;)
  {
    u32Status = xTaskNotifyWait( pdFALSE,          /* Don't clear bits on entry. */
                                 UINT32_MAX,        /* Clear all bits on exit. */
                                 &u32Notifications, /* Stores the notified value. */
                                 portMAX_DELAY );

    if( u32Status == pdPASS )
    {
      if( ( u32Notifications & MTR_EVT_GO ) != 0 )
      {
        if(xQueueReceive(gbl_sMotorDrivingQueueHdl, &sMotorCfg, 0u) == pdPASS)
        {
          // Motor configuration is in queue
          MotorController_Run(sMotorCfg);
        }
      }

      if( ( u32Notifications & MTR_EVT_STOP ) != 0 )
      {
        MotorController_Stop();
      }

      if( ( u32Notifications & MTR_EVT_ISALIVE ) != 0 )
      {
        SupervisorTask_MotorTaskIsAlive();
      }
    }
    else
    {
      // OS error if we are here
      Error_Handler(true, ERR_OS_MOTOR_TASK, ERR_TYPE_TIMEOUT);
    }
  }
}
