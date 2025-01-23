#include <stdbool.h>

#include "timer_pwm.h"

#include "stm32l5xx_ll_tim.h"
#include "stm32l5xx_ll_bus.h"

#define APB_TIM3_PRESCALER (1u)
#define TIMER_PWM_MAX_DUTY_CYCLE (100u)

/**
  @brief Setup the TimerPwm.
 */
void TimerPwm_Init(void)
{
  static bool loc_bInitialized = false;
  if(!loc_bInitialized)
  {
    loc_bInitialized = true;
    // Enable TIM3 clock
    LL_APB1_GRP1_EnableClock(LL_APB1_GRP1_PERIPH_TIM3);

    // Configure TIM3 for PWM mode
    uint32_t timer_clock = SystemCoreClock / 1; // Assuming no prescaler for simplicity
    uint32_t pwm_frequency = 1000; // 1 kHz PWM
    uint32_t prescaler = 0; // No prescaler
    uint32_t auto_reload = timer_clock / pwm_frequency - 1; // ARR for desired frequency

    LL_TIM_SetPrescaler(TIM3, prescaler);
    LL_TIM_SetAutoReload(TIM3, auto_reload);
    LL_TIM_SetCounterMode(TIM3, LL_TIM_COUNTERMODE_UP);
    LL_TIM_EnableARRPreload(TIM3);

    // Configure Channel 2 in PWM mode
    LL_TIM_OC_SetMode(TIM3, LL_TIM_CHANNEL_CH2, LL_TIM_OCMODE_PWM1);
    LL_TIM_OC_SetCompareCH2(TIM3, auto_reload / 2); // 50% duty cycle initially
    LL_TIM_OC_EnablePreload(TIM3, LL_TIM_CHANNEL_CH2);

    // Enable output
    LL_TIM_EnableAllOutputs(TIM3); // For advanced timers, not necessary for TIM3
  }
}


/**
  @brief Start TimerPwm.
 */
void TimerPwm_Activate(void)
{
  // Start Timer
  LL_TIM_EnableCounter(TIM3);

  // Enable Channel 2 Output
  LL_TIM_CC_EnableChannel(TIM3, LL_TIM_CHANNEL_CH2);
}

/**
  @brief Set duty cycle.
 */
inline void TimerPwm_SetDutyCycle(uint32_t arg_u32DutyCycle)
{
  uint32_t loc_u32PulseDuration;    /* Pulse duration */
  uint32_t loc_u32TimerPeriod;    /* PWM signal period */
  uint32_t loc_u32DutyCycle = arg_u32DutyCycle;    /* PWM duty cycle */

  if(loc_u32DutyCycle > TIMER_PWM_MAX_DUTY_CYCLE)
  {
    loc_u32DutyCycle = TIMER_PWM_MAX_DUTY_CYCLE;
  }

  /* PWM signal period is determined by the value of the auto-reload register */
  loc_u32TimerPeriod = LL_TIM_GetAutoReload(TIM3) + 1;

  /* Pulse duration is determined by the value of the compare register.       */
  /* Its value is calculated in order to match the requested duty cycle.      */
  loc_u32PulseDuration = (loc_u32DutyCycle*loc_u32TimerPeriod)/TIMER_PWM_MAX_DUTY_CYCLE;
  LL_TIM_OC_SetCompareCH2(TIM3, loc_u32PulseDuration);
}
