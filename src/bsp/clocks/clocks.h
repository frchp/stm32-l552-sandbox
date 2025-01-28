#ifndef _CLOCKS_H_
#define _CLOCKS_H_

#include <stdint.h>

/**
  @brief Enable LSI clock.
 */
void Clocks_EnableLSI(void);

/**
  @brief Enable given peripheral clock.
 */
void Clocks_Enable_AHB2_GRP1(uint32_t arg_u32Msk);

#endif // _CLOCKS_H_