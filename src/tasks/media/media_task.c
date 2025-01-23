#include "media_task.h"

/* Media task parameters*/
TaskHandle_t MediaTaskHandle;
StackType_t gbl_sStackMedia [MEDIA_TASK_STACK_SIZE];
StaticTask_t gbl_sTCBMedia;

void MediaTask(void *pvParameters)
{
  (void)pvParameters; // Avoid compiler warning for unused parameter

  // Create media task semaphore

  for (;;)
  {
    // Block for 500 ms
    vTaskDelay(pdMS_TO_TICKS(500));
  }
}
