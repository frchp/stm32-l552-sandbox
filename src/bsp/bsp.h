#ifndef _BSP_H_
#define _BSP_H_

#include <stdint.h>
#include "cmsis_compiler.h"

/**
  @brief Setup the BSP.
 */
void Bsp_Init (void);

/**
  @brief Activate the BSP.
 */
void Bsp_Activate (void);

/**
  @brief Enter critical section.
 */
static inline uint32_t Bsp_EnterCritical(void)
{
  uint32_t loc_u32irqState = __get_PRIMASK();
  __disable_irq();
  return loc_u32irqState;
}

/**
  @brief Exit critical section.
 */
static inline void Bsp_ExitCritical(uint32_t arg_u32irqState)
{
  if (arg_u32irqState == 0U) {
    __enable_irq();
  }
}

#endif // _BSP_H_