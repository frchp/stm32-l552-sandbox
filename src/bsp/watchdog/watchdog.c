#include "watchdog.h"
#include "stm32l5xx_ll_iwdg.h"
#include "stm32l5xx_ll_rcc.h"

/**
  @brief Setup the Watchdog.
 */
void Watchdog_Activate(void)
{
  /* Enable clock feeding IWDG (32kHz) */
  LL_RCC_LSI_Enable();
  while (!LL_RCC_LSI_IsReady())
  {
  }

  /* Configure the IWDG with window option disabled */
  /* (1) Enable the IWDG by writing 0x0000 CCCC in the IWDG_KR register */
  /* (2) Enable register access by writing 0x0000 5555 in the IWDG_KR register */
  /* (3) Write the IWDG prescaler by programming IWDG_PR from 0 to 7 - LL_IWDG_PRESCALER_4 (0) is lowest divider*/
  /* (4) Write the reload register (IWDG_RLR) */
  /* (5) Wait for the registers to be updated (IWDG_SR = 0x0000 0000) */
  /* (6) Refresh the counter value with IWDG_RLR (IWDG_KR = 0x0000 AAAA) */
  LL_IWDG_Enable(IWDG);                             /* (1) */
  LL_IWDG_EnableWriteAccess(IWDG);                  /* (2) */
  LL_IWDG_SetPrescaler(IWDG, LL_IWDG_PRESCALER_4);  /* (3) */
  LL_IWDG_SetReloadCounter(IWDG, 0xFEE); // 500ms   /* (4) */
  while (!LL_IWDG_IsReady(IWDG))                /* (5) */
  {
  }
  LL_IWDG_ReloadCounter(IWDG);                      /* (6) */
}

/**
  @brief Kick the dog.
 */
void Watchdog_Refresh(void)
{
  LL_IWDG_ReloadCounter(IWDG);
}
