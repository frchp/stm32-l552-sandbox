#include <stdint.h>

#include "error.h"
#include "system.h"
#include "watchdog.h"
#include "app.h"
#include "task_handler.h"

int main(void)
{
  Bsp_Init();
  Bsp_Activate();

  App_Init();

  TaskHandler_Init();

  while(1)
  {
    Watchdog_Refresh();
  }
}
