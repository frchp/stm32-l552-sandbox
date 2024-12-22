#ifndef _CPU_DELAY_H_
#define _CPU_DELAY_H_

#include "system_stm32l5xx.h"

static __inline void cpu_delay_WaitFor(uint32_t arg_u32Delay)
{
  volatile uint32_t loc_u32WaitIndex = 0U;
  loc_u32WaitIndex = ((arg_u32Delay * (SystemCoreClock / (100000 * 2))) / 10);
  while(loc_u32WaitIndex != 0)
  {
    loc_u32WaitIndex--;
  }
}

#endif // _CPU_DELAY_H_