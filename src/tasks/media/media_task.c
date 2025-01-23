#include <stdbool.h>

#include "media_task.h"

#include "error.h"
#include "order_handler.h"

#include "FreeRTOS.h"
#include "task.h"

/* Media task parameters*/
TaskHandle_t gbl_sMediaTaskHandle;
StackType_t gbl_sStackMedia [MEDIA_TASK_STACK_SIZE];
StaticTask_t gbl_sTCBMedia;

static bool gbl_bTaskInitialized = false;

void MediaTask_OnReception(void)
{
  BaseType_t xHigherPriorityTaskWoken = pdFALSE;
  if(gbl_bTaskInitialized)
  {
    if(xPortIsInsideInterrupt() != 0u)
    {
      // In interrupt context
      vTaskNotifyGiveFromISR(gbl_sMediaTaskHandle, &xHigherPriorityTaskWoken);
      portYIELD_FROM_ISR( xHigherPriorityTaskWoken );
    }
    else
    {
      xTaskNotifyGive(gbl_sMediaTaskHandle);
    }
  }
}

void MediaTask(void *arg_pvParameters)
{
  (void)arg_pvParameters; // Avoid compiler warning for unused parameter
  uint32_t u32NotifiedValue;

  gbl_bTaskInitialized = true;

  for (;;)
  {
    // Notification is used as counting semaphore for order_handler incoming
    u32NotifiedValue = ulTaskNotifyTake( pdFALSE /* value decremented */,
                                        portMAX_DELAY /* wait for ever for an incoming msg */);

    if( u32NotifiedValue > 0u )
    {
      // Message to handle
      OrderHandler_HandleRx();
    }
    else
    {
      Error_Handler();
    }
  }
}
