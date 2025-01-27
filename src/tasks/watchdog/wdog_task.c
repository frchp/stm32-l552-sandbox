#include "wdog_task.h"

#include "watchdog.h"
#include "board.h"
#include "stm32l5xx_ll_gpio.h"

/* Watchdog task parameters */
TaskHandle_t gbl_sWatchdogTaskHandle;
StackType_t gbl_sStackWdog [WDOG_TASK_STACK_SIZE];
StaticTask_t gbl_sTCBWdog;

void WatchdogTask(void *arg_pvParameters)
{
  (void)arg_pvParameters; // Avoid compiler warning for unused parameter

  for (;;)
  {
    // Block for 300 ms and refresh watchdog
    vTaskDelay(pdMS_TO_TICKS(300));
    LL_GPIO_TogglePin(BOARD_GPIO_LED_ALIVE_PORT, BOARD_GPIO_LED_ALIVE_PIN);
    Watchdog_Refresh();
  }
}
