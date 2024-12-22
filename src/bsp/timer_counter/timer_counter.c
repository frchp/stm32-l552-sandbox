#include "timer_counter.h"
#include "timer_counter_config.h"

#include "stm32l5xx_ll_tim.h"
#include "stm32l5xx_ll_bus.h"

static uint32_t gbl_u32Frequency = 0u;
static uint32_t gbl_u32NbPulses = 0u;

/**
  @brief Setup the TimerCounter.
 */
void TimerCounter_Init(void)
{
  NVIC_SetPriority(TIM1_CC_IRQn, 0);
  NVIC_EnableIRQ(TIM1_CC_IRQn);
  LL_APB2_GRP1_EnableClock(LL_APB2_GRP1_PERIPH_TIM1);
  LL_TIM_IC_SetActiveInput(TIM1, LL_TIM_CHANNEL_CH1, LL_TIM_ACTIVEINPUT_DIRECTTI);
  LL_TIM_IC_SetFilter(TIM1, LL_TIM_CHANNEL_CH1, LL_TIM_IC_FILTER_FDIV1);
  LL_TIM_IC_SetPrescaler(TIM1, LL_TIM_CHANNEL_CH1, LL_TIM_ICPSC_DIV1);
  LL_TIM_IC_SetPolarity(TIM1, LL_TIM_CHANNEL_CH1, LL_TIM_IC_POLARITY_RISING);
  LL_TIM_EnableIT_CC1(TIM1);
  LL_TIM_CC_EnableChannel(TIM1, LL_TIM_CHANNEL_CH1);
}

/**
  @brief Start TimerCounter.
 */
void TimerCounter_Activate(void)
{
  LL_TIM_OC_SetCompareCH1(TIM1, 0u); // Reset first value
  LL_TIM_EnableCounter(TIM1);
}

/**
  @brief Return last value of counted pulses.
 */
uint32_t TimerCounter_Get(void)
{
  return gbl_u32Frequency;
}

/**
  @brief Period has elapsed, compute count.
 */
void TimerCounter_TimeElapsed(void)
{
  uint32_t loc_u32NbPulses = 0u;
  uint32_t TIM1CLK, PSC, IC1PSC, IC1Polarity;

  if(gbl_u32NbPulses < TIMER_COUNTER_NB_PULSES)
  {
    // First pulses detected
    gbl_u32NbPulses++;
  }
  else
  {
    LL_TIM_OC_SetCompareCH1(TIM1, 0u);
    gbl_u32NbPulses = 0u;

    // Value can be computed directly as we reset count value every time
    loc_u32NbPulses = LL_TIM_IC_GetCaptureCH1(TIM1);

    TIM1CLK = SystemCoreClock;
    PSC = LL_TIM_GetPrescaler(TIM1);
    IC1PSC = __LL_TIM_GET_ICPSC_RATIO(LL_TIM_IC_GetPrescaler(TIM1, LL_TIM_CHANNEL_CH1));
    if (LL_TIM_IC_GetPolarity(TIM1, LL_TIM_CHANNEL_CH1) == LL_TIM_IC_POLARITY_BOTHEDGE)
    {
      IC1Polarity = 2;
    }
    else
    {
      IC1Polarity = 1;
    }
    gbl_u32Frequency = (TIM1CLK *IC1PSC) / (loc_u32NbPulses*(PSC+1)*IC1Polarity);
  }
}
