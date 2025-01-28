#include "stm32l552xx.h"
#include "clocks.h"

/**
  @brief Enable LSI clock.
 */
void Clocks_EnableLSI(void)
{
  SET_BIT(RCC->CSR, RCC_CSR_LSION);
  // Wait for clock activation
  while((READ_BIT(RCC->CSR, RCC_CSR_LSIRDY) == RCC_CSR_LSIRDY) == 0UL);
}
