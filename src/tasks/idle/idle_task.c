#include <stdbool.h>

#include "FreeRTOS.h"
#include "task.h"
#include "FreeRTOSConfig.h"

#include "stm32l5xx_ll_pwr.h"
#include "stm32l5xx_ll_cortex.h"
#include "stm32l5xx_ll_rtc.h"
#include "stm32l5xx_ll_rcc.h"
#include "core_cm33.h"

#include "system.h"

/* LPM_VARIANT : 
* 1 - use FreeRTOS standard
* 2 - just WFI
* 3 - application LPM
*/
#define LPM_VARIANT (3u)

#define LPM_MODE_SLEEP     1  /* CPU clock OFF, WFI */
#define LPM_MODE_LP_SLEEP  2  /* CPU clock OFF, Set LPR bit - WFI - Return from ISR */
#define LPM_MODE_STOP      3  /* All clocks OFF except LSI and LSE, Reg + WFI + Int, wake up pin - RTC - ext reset - wdog reset */
#define LPM_MODE_STANDBY   4  /* All clocks OFF except LSI and LSE, Reg + WFI + Int, wake up pin - RTC - ext reset - wdog reset */
#define LPM_MODE_SHUTDOWN  5  /* All clocks OFF except LSE, Reg + WFI + Int, wake up pin - RTC - ext reset - wdog reset */

#define LPM_MODE LPM_MODE_LP_SLEEP

#if (LPM_MODE == LPM_MODE_STOP)
  #error "LPM_MODE_STOP not implemented"
#elif (LPM_MODE == LPM_MODE_STOP)

#endif

#define LPM_WAKEUP_WFI      1
#define LPM_WAKEUP_WFE      2
#define LPM_WAKEUP LPM_WAKEUP_WFI

#define LPM_RTC_FREQUENCY (32768ul)
#define LPM_TICK_RATE_HZ  (1000ul)

static void Idle_PRV_SetupRTC(void);

/*
* @brief Implements IDLE hook from FreeRTOS.
*/
void vApplicationIdleHook(void)
{
  #if (configUSE_TICKLESS_IDLE == 0u)
    #if (LPM_VARIANT == 1u)
      // Nothing
    #elif (LPM_VARIANT == 2u)
      __asm volatile("wfi");
    #elif (LPM_VARIANT == 3u)
      Idle_PRV_EnterLPM();
    #endif
  #endif // (configUSE_TICKLESS_IDLE == 0u)
}

/*
* @brief Is called by FreeRTOS before entering sleep.
*/
void Idle_OnPreSleepProcessing(uint32_t xExpectedTime)
{
  // 1. Prepare
  #if (LPM_MODE == LPM_MODE_STANDBY) || (LPM_MODE == LPM_MODE_SHUTDOWN)
    // Can be wakeup timer or wakeup pin : (here PC13)
    LL_PWR_DisableWakeUpPin(LL_PWR_WAKEUP_PIN2);
    LL_PWR_ClearFlag_WU();
    LL_PWR_SetWakeUpPinPolarityLow(LL_PWR_WAKEUP_PIN2);
    LL_PWR_EnableWakeUpPin(LL_PWR_WAKEUP_PIN2);
    LL_PWR_ClearFlag_WU();
  #endif

  #if (LPM_MODE == LPM_MODE_STOP)
    // TODO : Setup EXTI line
  #endif

  // 2. Choose mode
  #if (LPM_MODE == LPM_MODE_SLEEP)
    LL_LPM_EnableSleep();
  #elif (LPM_MODE == LPM_MODE_LP_SLEEP)
    LL_LPM_EnableSleep();
    LL_PWR_EnableLowPowerRunMode();
  #elif (LPM_MODE == LPM_MODE_STOP)
    LL_PWR_SetPowerMode(LL_PWR_MODE_STOP1);
    LL_LPM_EnableDeepSleep();
  #elif (LPM_MODE == LPM_MODE_STANDBY)
    LL_PWR_SetPowerMode(LL_PWR_MODE_STANDBY);
    LL_PWR_SetSRAM2Retention(LL_PWR_SRAM2_NO_RETENTION);
    LL_LPM_EnableDeepSleep();
  #elif (LPM_MODE == LPM_MODE_SHUTDOWN)
    LL_PWR_SetPowerMode(LL_PWR_MODE_SHUTDOWN);
    LL_LPM_EnableDeepSleep();
  #endif
}

/*
* @brief Is called by FreeRTOS after entering sleep.
*/
void Idle_OnPostSleepProcessing(uint32_t xExpectedTime)
{
  // 3. Restore
  LL_PWR_DisableLowPowerRunMode(); // Disable in any case
  Bsp_InitClock();
}

/*
* @brief Is called by FreeRTOS during Idle if user defined tickless idle should be implemented.
*/
void Idle_TicklessIdleSleep(uint32_t xExpectedTime)
{
  static bool bFirstSleep = true;  // Set to true for first time called
  uint32_t ulReloadValue;
  uint32_t ulInterruptValues;

  if(bFirstSleep == true)
  {
    bFirstSleep = false;
    Idle_PRV_SetupRTC();
  }


  // Calculate the reload value based on expected idle time
  ulReloadValue = xExpectedTime * (LPM_RTC_FREQUENCY / LPM_TICK_RATE_HZ);

  eSleepModeStatus eSleepStatus = eTaskConfirmSleepModeStatus();
  if( eSleepStatus == eAbortSleep )
  {
    /* A task has been moved out of the Blocked state since this macro was
      executed, or a context switch is being held pending. Do not enter a
      sleep state. Restart the tick and exit the critical section. */
  }
  else
  {
    // Disable SysTick
    SysTick->CTRL &= !SysTick_CTRL_ENABLE_Msk;
    LL_SYSTICK_DisableIT();

    // Disable interrupts (critical section)
    ulInterruptValues = Bsp_EnterCritical();

    if( eSleepStatus == eNoTasksWaitingTimeout )
    {
      // Enter low-power STOP mode
      LL_PWR_SetPowerMode(LL_PWR_MODE_STOP1);
      LL_LPM_EnableDeepSleep();
      #if (LPM_WAKEUP == LPM_WAKEUP_WFI)
        __asm volatile("wfi");
      #elif (LPM_WAKEUP == LPM_WAKEUP_WFE)
        __asm volatile("wfe");
      #else
        #error "LPM_WAKEUP not defined"
      #endif
    }
    else
    {
      // Disable RTC Alarm A interrupt while configuring the alarm
      LL_RTC_DisableIT_ALRA(RTC);

      // Set the alarm for the calculated number of seconds
      LL_RTC_ALMA_SetSecond(RTC, (uint8_t)(ulReloadValue % 60));
      LL_RTC_ALMA_SetMinute(RTC, (uint8_t)((ulReloadValue / 60) % 60));
      LL_RTC_ALMA_SetHour(RTC, (uint8_t)((ulReloadValue / 3600) % 24));

      // Enable RTC Alarm A interrupt again
      LL_RTC_EnableIT_ALRA(RTC);

      // Enter low-power STOP mode
      LL_PWR_SetPowerMode(LL_PWR_MODE_STOP1);
      LL_LPM_EnableDeepSleep();
      #if (LPM_WAKEUP == LPM_WAKEUP_WFI)
        __asm volatile("wfi");
      #elif (LPM_WAKEUP == LPM_WAKEUP_WFE)
        __asm volatile("wfe");
      #else
        #error "LPM_WAKEUP not defined"
      #endif
    }
  }
  // After waking up, the RTC interrupt will trigger and resume from here
  vTaskStepTick( xExpectedTime );

  // Re-enable interrupts and resume normal operation
  Bsp_ExitCritical(ulInterruptValues);

  // Enable SysTick
  SysTick->CTRL |= SysTick_CTRL_ENABLE_Msk;
  LL_SYSTICK_EnableIT();
}

static void Idle_PRV_SetupRTC(void)
{
  // Enable the LSE (Low-Speed External) oscillator for RTC
  LL_RCC_LSE_Enable();
  while (!LL_RCC_LSE_IsReady());  // Wait for LSE to stabilize

  // Enable RTC peripheral clock
  LL_RCC_SetRTCClockSource(LL_RCC_RTC_CLKSOURCE_LSE);
  LL_RCC_EnableRTC();

  // Configure RTC
  LL_RTC_InitTypeDef rtc_init = {0};
  rtc_init.HourFormat = LL_RTC_HOURFORMAT_24HOUR;  // 24-hour format
  rtc_init.AsynchPrescaler = 0x7F;                    // Asynchronous prescaler
  rtc_init.SynchPrescaler = 0xFF;                     // Synchronous prescaler

  LL_RTC_Init(RTC, &rtc_init);

  // Enable RTC Alarm A interrupt
  LL_RTC_EnableIT_ALRA(RTC);

  // Configure RTC Alarm A to trigger every second
  LL_RTC_ALMA_Enable(RTC);
  LL_RTC_ALMA_SetMask(RTC, LL_RTC_ALMA_MASK_NONE);  // No masking
  LL_RTC_ALMA_SetSubSecond(RTC, 0);  // No subsecond
  LL_RTC_DATE_SetMonth(RTC, LL_RTC_MONTH_JANUARY);     // Set alarm month
  LL_RTC_ALMA_SetDay(RTC, 1);       // Set alarm day
  LL_RTC_ALMA_SetHour(RTC, 0);     // Set alarm hours
  LL_RTC_ALMA_SetMinute(RTC, 0);   // Set alarm minutes
  LL_RTC_ALMA_SetSecond(RTC, 1);   // Set alarm seconds (1 second interval)
  LL_RTC_ALMA_SetWeekDay(RTC, LL_RTC_WEEKDAY_MONDAY);

  // Enable alarm
  LL_RTC_ALMA_Enable(RTC);
}
