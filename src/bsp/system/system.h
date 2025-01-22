#ifndef _SYSTEM_H_
#define _SYSTEM_H_

#include <stdint.h>
#include "cmsis_compiler.h"

/**
  @brief Setup the microcontroller system.

   Initialize the System and update the SystemCoreClock variable.
 */
void SystemInit (void);

/**
  @brief Update clocks based on register value.
 */
void SystemCoreClockUpdate(void);

/**
  @brief Setup the BSP.
 */
void Bsp_Init (void);

/**
  @brief Setup the BSP.
 */
void Bsp_Activate (void);

/**
  @brief Setup the clock tree.
 */
void Bsp_InitClock(void);

/**
  @brief Enter critical section.
 */
static inline uint32_t Bsp_EnterCritical(void)
{
  uint32_t irqState = __get_PRIMASK();
  __disable_irq();
  return irqState;
}

/**
  @brief Exit critical section.
 */
static inline void Bsp_ExitCritical(uint32_t irqState)
{
  if (irqState == 0U) {
    __enable_irq();
  }
}


#endif // _SYSTEM_H_