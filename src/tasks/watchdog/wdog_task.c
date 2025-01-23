#include "wdog_task.h"

#include "watchdog.h"

/* Watchdog task parameters */
TaskHandle_t gbl_sWatchdogTaskHandle;
StackType_t gbl_sStackWdog [WDOG_TASK_STACK_SIZE];
StaticTask_t gbl_sTCBWdog;

void WatchdogTask(void *arg_pvParameters)
{
    (void)arg_pvParameters; // Avoid compiler warning for unused parameter

    for (;;)
    {
        // Block for 1000 ms and refresh watchdog
        vTaskDelay(pdMS_TO_TICKS(1000));
        Watchdog_Refresh();
    }
}
