#include <stdbool.h>

#include "watchdog.h"

#include "reg_macro.h"
#include "clocks.h"

#include "stm32l5xx_ll_iwdg.h"
#include "stm32l5xx_ll_rcc.h"

#define IWDG_KEY_RELOAD                 0x0000AAAAU
#define IWDG_KEY_ENABLE                 0x0000CCCCU
#define IWDG_KEY_WR_ACCESS_ENABLE       0x00005555U
#define IWDG_KEY_WR_ACCESS_DISABLE      0x00000000U

#define IWDG_PRESCALER_4                0x00000000U                           /*!< Divider by 4   */
#define IWDG_PRESCALER_8                (IWDG_PR_PR_0)                        /*!< Divider by 8   */
#define IWDG_PRESCALER_16               (IWDG_PR_PR_1)                        /*!< Divider by 16  */
#define IWDG_PRESCALER_32               (IWDG_PR_PR_1 | IWDG_PR_PR_0)         /*!< Divider by 32  */
#define IWDG_PRESCALER_64               (IWDG_PR_PR_2)                        /*!< Divider by 64  */
#define IWDG_PRESCALER_128              (IWDG_PR_PR_2 | IWDG_PR_PR_0)         /*!< Divider by 128 */
#define IWDG_PRESCALER_256              (IWDG_PR_PR_2 | IWDG_PR_PR_1)         /*!< Divider by 256 */

#define IWDG_PERIOD_CNT                 (0xFEE) /* 500 ms */

/**
  @brief Setup the Watchdog.
 */
void Watchdog_Activate(void)
{
  static bool loc_bInitialized = false;
  if(!loc_bInitialized)
  {
    loc_bInitialized = true;
    /* Enable clock feeding IWDG (32kHz) */
    Clocks_EnableLSI();

    /* Configure the IWDG with window option disabled */
    WRITE_REG(IWDG->KR, IWDG_KEY_ENABLE);             // Enable the IWDG by writing 0x0000 CCCC in the IWDG_KR register
    WRITE_REG(IWDG->KR, IWDG_KEY_WR_ACCESS_ENABLE);   // Enable register access by writing 0x0000 5555 in the IWDG_KR register
    WRITE_REG(IWDG->PR, IWDG_PR_PR & IWDG_PRESCALER_4); // Write the IWDG prescaler by programming IWDG_PR from 0 to 7 - IWDG_PRESCALER_4 (0) is lowest divider
    WRITE_REG(IWDG->RLR, IWDG_RLR_RL & IWDG_PERIOD_CNT);// Write the reload register
    while(READ_BIT(IWDG->SR, IWDG_SR_PVU | IWDG_SR_RVU | IWDG_SR_WVU) != 0U);
    Watchdog_Refresh();
  }
}

/**
  @brief Kick the dog.
 */
void Watchdog_Refresh(void)
{
  WRITE_REG(IWDG->KR, LL_IWDG_KEY_RELOAD);
}
