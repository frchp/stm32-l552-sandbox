#include <stdbool.h>

#include "watchdog.h"

#include "stm32l5xx_ll_iwdg.h"
#include "stm32l5xx_ll_rcc.h"

/**
  @brief Setup the Watchdog.
 */
void Watchdog_Activate(void)
{
  static bool bInitialized = false;
  if(!bInitialized)
  {
    bInitialized = true;
    /* Enable clock feeding IWDG (32kHz) */
    LL_RCC_LSI_Enable();
    while (!LL_RCC_LSI_IsReady());

    /* Configure the IWDG with window option disabled */
    LL_IWDG_Enable(IWDG);                             // Enable the IWDG by writing 0x0000 CCCC in the IWDG_KR register
    LL_IWDG_EnableWriteAccess(IWDG);                  // Enable register access by writing 0x0000 5555 in the IWDG_KR register
    LL_IWDG_SetPrescaler(IWDG, LL_IWDG_PRESCALER_4);  // Write the IWDG prescaler by programming IWDG_PR from 0 to 7 - LL_IWDG_PRESCALER_4 (0) is lowest divider
    LL_IWDG_SetReloadCounter(IWDG, 0xFEE); // 500ms   // Write the reload register (IWDG_RLR)
    while (!LL_IWDG_IsReady(IWDG));                   // Wait for the registers to be updated (IWDG_SR = 0x0000 0000)
    LL_IWDG_ReloadCounter(IWDG);                      // Refresh the counter value with IWDG_RLR (IWDG_KR = 0x0000 AAAA)
  }
}

/**
  @brief Kick the dog.
 */
void Watchdog_Refresh(void)
{
  LL_IWDG_ReloadCounter(IWDG);
}
