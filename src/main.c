#include <stdint.h>

#include "error.h"
#include "system.h"
#include "watchdog.h"

int main(void)
{
  Bsp_Init();
  
  Bsp_Activate();

  while(1)
  {
    Watchdog_Refresh();
  }
}
