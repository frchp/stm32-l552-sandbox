#include "clocks.h"

#include "stm32l552xx.h"
#include "reg_macro.h"

/**
  @brief Enable LSI clock.
 */
void Clocks_EnableLSI(void)
{
  SET_BIT(RCC->CSR, RCC_CSR_LSION);
  // Wait for clock activation
  while((READ_BIT(RCC->CSR, RCC_CSR_LSIRDY) == RCC_CSR_LSIRDY) == 0UL);
}

/**
  @brief Enable given peripheral clock.
 */
void Clocks_Enable_AHB2_GRP1(uint32_t arg_u32Msk)
{
  SET_BIT(RCC->AHB2ENR, arg_u32Msk);
  /* Delay after an RCC peripheral clock enabling */
  (void)READ_BIT(RCC->AHB2ENR, arg_u32Msk);
}
