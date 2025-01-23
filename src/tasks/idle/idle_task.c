#include <stdbool.h>

#include "FreeRTOS.h"
#include "task.h"
#include "FreeRTOSConfig.h"

#include "stm32l5xx_ll_pwr.h"
#include "stm32l5xx_ll_cortex.h"
#include "stm32l5xx_ll_lptim.h"
#include "stm32l5xx_ll_rcc.h"
#include "stm32l5xx_ll_bus.h"
#include "core_cm33.h"

#include "interrupts.h"
#include "system.h"

/**
 * LPM_MODE_SLEEP     CPU clock OFF, WFI
 * LPM_MODE_LP_SLEEP  CPU clock OFF, Set LPR bit - WFI - Return from ISR
 * LPM_MODE_STOP      All clocks OFF except LSI and LSE, Reg + WFI + Int, wake up pin - RTC - ext reset - wdog reset
 * LPM_MODE_STANDBY   All clocks OFF except LSI and LSE, Reg + WFI + Int, wake up pin - RTC - ext reset - wdog reset
 * LPM_MODE_SHUTDOWN  All clocks OFF except LSE, Reg + WFI + Int, wake up pin - RTC - ext reset - wdog reset
 */

/**
 * FreeRTOS idle :
 * 1. Use idle hook : own LPM implementation, check LPM bitfield to enable entry in LPM
 * enter/exit LPM functions, call for WFI/WFE, load timer to sleep, disable systick, adjust tick count on exit
 * 2. Use FreeRTOS tickless idle : configUSE_TICKLESS_IDLE = 1 : nothing to do
 * 3. Use User tickless idle : configUSE_TICKLESS_IDLE > 1 : similar to idle hook except idle hook can wake up on next tick
 */

#if (configUSE_TICKLESS_IDLE == 2)

static uint32_t gbl_u32LPTIMCountsPerTick = 0u;

// Peripheral selection
#define LPTIM_CLOCK_HZ     ( 32000u )
#define LPTIM_MAX_ARR      (0xFFFFu)

static void Idle_PRV_SetupLPTIM(void);

/*
* @brief Is called by FreeRTOS during Idle if user defined tickless idle should be implemented.
*/
void vPortSuppressTicksAndSleep(uint32_t xExpectedIdleTime)
{
  uint32_t loc_u32LPTIMCurrentCount, loc_u32SleepTime, loc_u32WakeupTime;
  static bool loc_bInit = false;

  if(!loc_bInit)
  {
    loc_bInit = true;

    // Enter STOP1 mode (lowest power while retaining RAM/Peripherals)
    LL_PWR_SetPowerMode(LL_PWR_MODE_STOP1);
    LL_LPM_EnableDeepSleep();

    Idle_PRV_SetupLPTIM();
  }

  /* Enter a critical section but don't use the taskENTER_CRITICAL()
    * method as that will mask interrupts that should exit sleep mode. */
  __asm volatile( "cpsid i" ::: "memory" );
  __asm volatile( "dsb" );
  __asm volatile( "isb" );

  if (eTaskConfirmSleepModeStatus() == eAbortSleep)
  {
    __asm volatile( "cpsie i" ::: "memory" );
    return;
  }

  // Disable SysTick to prevent it from firing during sleep
  SysTick->CTRL &= ~SysTick_CTRL_ENABLE_Msk;

  // Calculate sleep time in LPTIM ticks
  loc_u32SleepTime = xExpectedIdleTime * gbl_u32LPTIMCountsPerTick;
  if (loc_u32SleepTime > LPTIM_MAX_ARR)
  {
    loc_u32SleepTime = LPTIM_MAX_ARR;
  }

  // Read current LPTIM counter value
  loc_u32LPTIMCurrentCount = LL_LPTIM_GetCounter(LPTIM1);
  loc_u32WakeupTime = loc_u32LPTIMCurrentCount + loc_u32SleepTime;

  // Set new compare value to wake up
  LL_LPTIM_SetCompare(LPTIM1, (loc_u32WakeupTime & LPTIM_MAX_ARR));

  // Clear interrupt flags before sleeping to avoid missed events
  LL_LPTIM_ClearFLAG_CMPM(LPTIM1);

  // Enable LPTIM compare interrupt to wake up the system
  LL_LPTIM_EnableIT_CMPM(LPTIM1);

  __asm volatile( "dsb" );
  __asm volatile( "wfi" );
  __asm volatile( "isb" );

  // Re-enable interrupts to allow the interrupt that brought the MCU out of sleep mode to execute immediately. */
  __asm volatile( "cpsie i" ::: "memory" );
  __asm volatile( "dsb" );
  __asm volatile( "isb" );

  // Disable interrupts again to not increase any slippage
  __asm volatile( "cpsid i" ::: "memory" );
  __asm volatile( "dsb" );
  __asm volatile( "isb" );

  // Read counter after wakeup
  uint32_t loc_u32CounterAfterWakeup = LL_LPTIM_GetCounter(LPTIM1);
  // Calculate elapsed time
  uint32_t loc_u32ElapsedCounts = (loc_u32CounterAfterWakeup - loc_u32LPTIMCurrentCount) & 0xFFFF;
  TickType_t loc_xElapsedTicks = loc_u32ElapsedCounts / gbl_u32LPTIMCountsPerTick;

  // Adjust FreeRTOS tick count
  vTaskStepTick(loc_xElapsedTicks);
  /**
   * If the RTOS is configured to use tickless idle functionality then the tick interrupt will be stopped, 
   * and the microcontroller placed into a low power state, whenever the Idle task is the only task able to execute. 
   * Upon exiting the low power state the tick count value must be corrected to account for the time that passed while it was stopped.
   */
  // xTaskCatchUpTicks(loc_xElapsedTicks);
  /**
   * Corrects the tick count value after the application code has held interrupts disabled for an extended period.
   * This function is similar to vTaskStepTick(), however, unlike vTaskStepTick(), this function may move the tick count 
   * forward past a time at which a task should be remove from the blocked state. That means xTaskCatchUpTicks() may remove tasks 
   * from the blocked state.
   */

  // Reset count for next sleep
  LL_LPTIM_ResetCounter(LPTIM1);
  while(LL_LPTIM_GetCounter(LPTIM1) != 0u);
  LL_LPTIM_DisableIT_CMPM(LPTIM1);

  // Restore nominal SysTick operation
  SysTick->VAL = 0;
  SysTick->CTRL |= SysTick_CTRL_ENABLE_Msk;

  // Exit critical section after resuming normal operation
  __asm volatile( "cpsie i" ::: "memory" );
}

static void Idle_PRV_SetupLPTIM(void)
{
  // Enable LPTIM1 clock
  LL_APB1_GRP1_EnableClock(LL_APB1_GRP1_PERIPH_LPTIM1);
  LL_RCC_LSI_Enable();
  while (LL_RCC_LSI_IsReady() != 1);
  LL_RCC_SetLPTIMClockSource(LL_RCC_LPTIM1_CLKSOURCE_LSI);

  // Reset LPTIM1 per errata
  LL_APB1_GRP1_ForceReset(LL_APB1_GRP1_PERIPH_LPTIM1);
  LL_APB1_GRP1_ReleaseReset(LL_APB1_GRP1_PERIPH_LPTIM1);

  // Stop LPTIM in debug
  DBGMCU->APB1FZR1 |= DBGMCU_APB1FZR1_DBG_LPTIM1_STOP;

  // Calculate the number of LPTIM counts per FreeRTOS tick
  gbl_u32LPTIMCountsPerTick = LPTIM_CLOCK_HZ / configTICK_RATE_HZ;

  // Configure LPTIM1
  LL_LPTIM_Enable(LPTIM1);

  LL_LPTIM_SetPrescaler(LPTIM1, LL_LPTIM_PRESCALER_DIV1);
  LL_LPTIM_SetAutoReload(LPTIM1, LPTIM_MAX_ARR);

  Interrupts_Enable(INT_LPTIM);

  // Start the timer in continuous mode
  LL_LPTIM_StartCounter(LPTIM1, LL_LPTIM_OPERATING_MODE_CONTINUOUS);
}

#endif

#if (configUSE_IDLE_HOOK == 1)
void vApplicationIdleHook(void)
{
  __NOP();
}
#endif
