#include <stdbool.h>

#include "motor_task.h"

#include "error.h"
#include "FreeRTOS.h"
#include "task.h"
#include "queue.h"

// Task specific application
#include "motor_controller.h"

/* Motor task parameters */
TaskHandle_t MotorDrivingTaskHandle;
StackType_t gbl_sStackMotor [MOTOR_TASK_STACK_SIZE];
StaticTask_t gbl_sTCBMotor;

/* Queue motor config */
#define MTR_QUEUE_LENGTH            5u
#define MTR_QUEUE_ITEM_SIZE         sizeof( Motor_Control_t )
static StaticQueue_t MotorDrivingCfgQueue;
uint8_t au8MotorDrivingCfgQueueStorage[ MTR_QUEUE_LENGTH * MTR_QUEUE_ITEM_SIZE ];
QueueHandle_t MotorDrivingQueueHdl;

static bool gbl_bTaskInitialized = false;

void MotorDrivingTask_Notify(uint32_t arg_u32Evt)
{
  BaseType_t xHigherPriorityTaskWoken = pdFALSE;
  if(gbl_bTaskInitialized)
  {
    if(xPortIsInsideInterrupt() != 0u)
    {
        // In interrupt context
        xTaskNotifyFromISR( MotorDrivingTaskHandle,
                        arg_u32Evt,
                        eSetBits,
                        &xHigherPriorityTaskWoken );
        portYIELD_FROM_ISR( xHigherPriorityTaskWoken );
    }
    else
    {
        xTaskNotify( MotorDrivingTaskHandle,
                            arg_u32Evt,
                            eSetBits );
    }
  }
}

void MotorDrivingTask_SetCfg(void *arg_vMtrCfg)
{
  // TODO : fill queue
  BaseType_t xHigherPriorityTaskWoken = pdFALSE;
  if(gbl_bTaskInitialized)
  {
    if(xPortIsInsideInterrupt() != 0u)
    {
      // In interrupt context
      xQueueSendToBackFromISR(MotorDrivingQueueHdl,
                    arg_vMtrCfg,
                    &xHigherPriorityTaskWoken);
      portYIELD_FROM_ISR( xHigherPriorityTaskWoken );
    }
    else
    {
      xQueueSendToBack(MotorDrivingQueueHdl,
                    arg_vMtrCfg,
                    0u);
    }
  }
}

void MotorDrivingTask(void *pvParameters)
{
  (void)pvParameters; // Avoid compiler warning for unused parameter
  uint32_t u32Notifications = 0ul;
  uint32_t u32Status = pdPASS;
  Motor_Control_t sMotorCfg;

  MotorDrivingQueueHdl = xQueueCreateStatic( MTR_QUEUE_LENGTH,
                                 MTR_QUEUE_ITEM_SIZE,
                                 au8MotorDrivingCfgQueueStorage,
                                 &MotorDrivingCfgQueue );
  configASSERT( MotorDrivingQueueHdl );

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
        MotorController_Run();
      }

      if( ( u32Notifications & MTR_EVT_CFG ) != 0 )
      {
        if(xQueueReceive(MotorDrivingQueueHdl, &sMotorCfg, 0u) == pdPASS)
        {
          // Motor configuration is in queue
          MotorController_Configure(sMotorCfg);
        }
      }

      if( ( u32Notifications & MTR_EVT_STOP ) != 0 )
      {
        MotorController_Stop();
      }
    }
    else
    {
      Error_Handler();
    }
  }
}
