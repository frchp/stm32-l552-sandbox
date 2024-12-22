#include "timer_pwm.h"

#include "stm32l5xx_ll_tim.h"
#include "stm32l5xx_ll_bus.h"

#define APB_TIM1_PRESCALER (1u)

static uint32_t gbl_u32MeasuredDutyCycle = 0u;

/**
  @brief Setup the TimerPwm.
 */
void TimerPwm_Init(void)
{
  NVIC_SetPriority(TIM1_CC_IRQn, 0);
  NVIC_EnableIRQ(TIM1_CC_IRQn);
  
  LL_APB2_GRP1_EnableClock(LL_APB2_GRP1_PERIPH_TIM1); 
  
  /* Reset value is LL_TIM_COUNTERMODE_UP */
  LL_TIM_SetCounterMode(TIM1, LL_TIM_COUNTERMODE_UP);
  
  /* Set the pre-scaler value to have TIM1 counter clock equal to 20 kHz */
  LL_TIM_SetPrescaler(TIM1, __LL_TIM_CALC_PSC(SystemCoreClock, 20000));
  
  /* Enable TIM1_ARR register preload. Writing to or reading from the         */
  /* auto-reload register accesses the preload register. The content of the   */
  /* preload register are transferred into the shadow register at each update */
  /* event (UEV).                                                             */  
  LL_TIM_EnableARRPreload(TIM1);
  
  /* Set the auto-reload value to have a counter frequency of 100 Hz */
  /* TIM1CLK = SystemCoreClock / (APB prescaler & multiplier)               */
  LL_TIM_SetAutoReload(TIM1, __LL_TIM_CALC_ARR(SystemCoreClock/APB_TIM1_PRESCALER, LL_TIM_GetPrescaler(TIM1), 100));
  
  /*********************************/
  /* Output waveform configuration */
  /*********************************/
  /* Set output mode */
  /* Reset value is LL_TIM_OCMODE_FROZEN */
  LL_TIM_OC_SetMode(TIM1, LL_TIM_CHANNEL_CH1, LL_TIM_OCMODE_PWM1);
  
  /* Reset value is LL_TIM_OCPOLARITY_HIGH */
  LL_TIM_OC_SetPolarity(TIM1, LL_TIM_CHANNEL_CH1, LL_TIM_OCPOLARITY_HIGH);
  
  /* Set compare value to half of the counter period (50% duty cycle ) */
  LL_TIM_OC_SetCompareCH1(TIM1, ( (LL_TIM_GetAutoReload(TIM1) + 1 ) / 2));
  
  /* Enable TIM1_CCR2 register preload. Read/Write operations access the      */
  /* preload register. TIM1_CCR2 preload value is loaded in the active        */
  /* at each update event.                                                    */
  LL_TIM_OC_EnablePreload(TIM1, LL_TIM_CHANNEL_CH1);
  
  /* Enable the capture/compare interrupt for channel 1*/
  LL_TIM_EnableIT_CC1(TIM1);
  
  /* Enable output channel 2 */
  LL_TIM_CC_EnableChannel(TIM1, LL_TIM_CHANNEL_CH1);
}

/**
  @brief Start TimerPwm.
 */
void TimerPwm_Activate(void)
{
  /* Enable counter */
  LL_TIM_EnableCounter(TIM1);
  
  /* Force update generation */
  LL_TIM_GenerateEvent_UPDATE(TIM1);
}

/**
  @brief Set duty cycle.
 */
inline void TimerPwm_SetDutyCycle(uint32_t arg_u32DutyCycle)
{
  uint32_t loc_u32PulseDuration;    /* Pulse duration */
  uint32_t loc_u32TimerPeriod;    /* PWM signal period */
  
  /* PWM signal period is determined by the value of the auto-reload register */
  loc_u32TimerPeriod = LL_TIM_GetAutoReload(TIM1) + 1;
  
  /* Pulse duration is determined by the value of the compare register.       */
  /* Its value is calculated in order to match the requested duty cycle.      */
  loc_u32PulseDuration = (arg_u32DutyCycle*loc_u32TimerPeriod)/100;
  LL_TIM_OC_SetCompareCH1(TIM1, loc_u32PulseDuration);
}

/**
  @brief Interrupt callback.
 */
void TimerPwm_Callback(void)
{
  uint32_t loc_u32CNTval = LL_TIM_GetCounter(TIM1);
  uint32_t loc_u32ARRval = LL_TIM_GetAutoReload(TIM1);
  
  if (LL_TIM_OC_GetCompareCH1(TIM1) > loc_u32ARRval )
  {
    /* If capture/compare setting is greater than autoreload, there is a counter overflow and counter restarts from 0.
       Need to add full period to counter value (ARR+1)  */
    loc_u32CNTval = loc_u32CNTval + loc_u32ARRval + 1;
  }

  gbl_u32MeasuredDutyCycle = (loc_u32CNTval * 100) / ( loc_u32ARRval + 1 );

}
