#include <stdint.h>

#include "bsp.h"
#include "app.h"
#include "task_handler.h"

int main(void)
{
  Bsp_Init();
  Bsp_Activate();

  App_Init();

  TaskHandler_Init();
  TaskHandler_StartOS();

  /* OS is started now, we should not be here */
}
