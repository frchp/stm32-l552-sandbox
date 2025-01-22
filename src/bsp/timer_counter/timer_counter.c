#include <stddef.h>
#include <stdbool.h>

#include "timer_counter.h"
#include "timer_counter_config.h"

#include "interrupts.h"

#include "stm32l5xx_ll_tim.h"
#include "stm32l5xx_ll_bus.h"
#include "stm32l5xx_ll_dma.h"

static uint32_t gbl_au32PulsesDetected[TIMER_COUNTER_NB_PULSES];
static fpCounterCallback gbl_fpListener = NULL;

static const uint32_t TIM_COUNTER_FREQ_HZ = 1000000ul; // 1us resolution

static void TimerCounter_PRV_InitDMA(void);

/**
  @brief Setup the TimerCounter.
 */
void TimerCounter_Init(void)
{
  static bool bInitialized = false;
  if(!bInitialized)
  {
    bInitialized = true;
    // Init DMA transfer from TIM1 to saved pulses values
    TimerCounter_PRV_InitDMA();

    // Enable clock
    LL_APB2_GRP1_EnableClock(LL_APB2_GRP1_PERIPH_TIM1);

    // Set TIM1 prescaler to create a time base
    LL_TIM_SetPrescaler(TIM1, __LL_TIM_CALC_PSC(SystemCoreClock, TIM_COUNTER_FREQ_HZ));
    LL_TIM_SetCounterMode(TIM1, LL_TIM_COUNTERMODE_UP);
    LL_TIM_EnableARRPreload(TIM1);
    LL_TIM_SetAutoReload(TIM1, 0xFFFFFFFF); // Maximum ARR for extended timing

    // Configure TIM1 Channel 1 in input capture mode
    LL_TIM_IC_SetActiveInput(TIM1, LL_TIM_CHANNEL_CH1, LL_TIM_ACTIVEINPUT_DIRECTTI);
    LL_TIM_IC_SetPrescaler(TIM1, LL_TIM_CHANNEL_CH1, LL_TIM_ICPSC_DIV1);
    LL_TIM_IC_SetFilter(TIM1, LL_TIM_CHANNEL_CH1, LL_TIM_IC_FILTER_FDIV1);
    LL_TIM_IC_SetPolarity(TIM1, LL_TIM_CHANNEL_CH1, LL_TIM_IC_POLARITY_RISING);  LL_TIM_EnableIT_CC1(TIM1);
    
    // Enable DMA request on TIM1_CH1 capture, no need for interrupt
    LL_TIM_EnableDMAReq_CC1(TIM1);
    
    LL_TIM_CC_EnableChannel(TIM1, LL_TIM_CHANNEL_CH1);
  }
}

/**
  @brief Start TimerCounter.
 */
void TimerCounter_Activate(void)
{
  LL_TIM_EnableCounter(TIM1);
}

/**
  @brief Return TimerCounter frequency.
 */
uint32_t TimerCounter_GetTimerFrequency(void)
{
  return TIM_COUNTER_FREQ_HZ;
}

/**
  @brief All pulses are detected, notify listeners.
 */
void TimerCounter_AllPulsesDetected(void)
{
  if (gbl_fpListener != NULL)
  {
    gbl_fpListener(gbl_au32PulsesDetected, TIMER_COUNTER_NB_PULSES);
  }
}

/**
  @brief Attach listeners.
 */
void TimerCounter_Attach(fpCounterCallback arg_fpListener)
{
  if(arg_fpListener != NULL)
  {
    gbl_fpListener = arg_fpListener;
  }
}

static void TimerCounter_PRV_InitDMA(void)
{
  // Enable DMA1 clock
  LL_AHB1_GRP1_EnableClock(LL_AHB1_GRP1_PERIPH_DMA1);

  // Configure DMA for TIM1_CH1 capture
  LL_DMA_SetPeriphRequest(DMA1, LL_DMA_CHANNEL_2, LL_DMAMUX_REQ_TIM1_CH1);
  LL_DMA_SetDataTransferDirection(DMA1, LL_DMA_CHANNEL_2, LL_DMA_DIRECTION_PERIPH_TO_MEMORY);
  LL_DMA_SetPeriphIncMode(DMA1, LL_DMA_CHANNEL_2, LL_DMA_PERIPH_NOINCREMENT);
  LL_DMA_SetMemoryIncMode(DMA1, LL_DMA_CHANNEL_2, LL_DMA_MEMORY_INCREMENT);
  LL_DMA_SetPeriphSize(DMA1, LL_DMA_CHANNEL_2, LL_DMA_PDATAALIGN_WORD);
  LL_DMA_SetMemorySize(DMA1, LL_DMA_CHANNEL_2, LL_DMA_MDATAALIGN_WORD);
  LL_DMA_SetDataLength(DMA1, LL_DMA_CHANNEL_2, TIMER_COUNTER_NB_PULSES);
  LL_DMA_SetMode(DMA1, LL_DMA_CHANNEL_2, LL_DMA_MODE_CIRCULAR);
  LL_DMA_ConfigAddresses(DMA1, LL_DMA_CHANNEL_2,
                          (uint32_t)&TIM1->CCR1,
                          (uint32_t)gbl_au32PulsesDetected,
                          LL_DMA_GetDataTransferDirection(DMA1, LL_DMA_CHANNEL_2));

  // Enable DMA interrupts for transfer complete and half-transfer
  LL_DMA_EnableIT_TC(DMA1, LL_DMA_CHANNEL_2);

  // Enable DMA Channel
  LL_DMA_EnableChannel(DMA1, LL_DMA_CHANNEL_2);

  // Configure NVIC for DMA1_Channel2 interrupts
  Interrupts_Enable(INT_DMA_CH2);
}