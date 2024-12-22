#include <stddef.h>

#include "adc.h"
#include "adc_config.h"

#include "cpu_delay.h"
#include "error.h"

#include "stm32l5xx_ll_dma.h"
#include "stm32l5xx_ll_tim.h"
#include "stm32l5xx_ll_rcc.h"
#include "stm32l5xx_ll_bus.h"
#include "stm32l5xx_ll_adc.h"

static uint32_t gbl_au32AdcValues[ADC_NB_SIGNALS];
static SignalConfig_t gbl_asAdcSignals[ADC_NB_SIGNALS];

#define ADC_DELAY_CALIB_ENABLE_CPU_CYCLES  (LL_ADC_DELAY_CALIB_ENABLE_ADC_CYCLES * 32u)
#define TIMER_FREQUENCY (200u)
#define TIMER_FREQUENCY_RANGE_MIN      (1UL)
#define TIMER_PRESCALER_MAX_VALUE      (0xFFFF - 1UL)

void Adc_PRV_InitDMA(void);
void Adc_PRV_InitCommon(void);
void Adc_PRV_InitSamplingTimer(void);

/**
  @brief Setup the ADC with automatic DMA transfer.
 */
void Adc_Init(const SignalConfig_t* arg_psSignal)
{
  LL_ADC_InitTypeDef ADC_InitStruct = {0};
  LL_ADC_REG_InitTypeDef ADC_REG_InitStruct = {0};

  LL_RCC_SetADCClockSource(LL_RCC_ADC_CLKSOURCE_SYSCLK);
  LL_AHB2_GRP1_EnableClock(LL_AHB2_GRP1_PERIPH_ADC);

  Adc_PRV_InitDMA();

  NVIC_SetPriority(ADC1_2_IRQn, 0);
  NVIC_EnableIRQ(ADC1_2_IRQn);

  ADC_InitStruct.Resolution = LL_ADC_RESOLUTION_12B;
  ADC_InitStruct.DataAlignment = LL_ADC_DATA_ALIGN_RIGHT;
  ADC_InitStruct.LowPowerMode = LL_ADC_LP_MODE_NONE;
  LL_ADC_Init(ADC1, &ADC_InitStruct);

  // Init REG : start by TIM2
  ADC_REG_InitStruct.TriggerSource = LL_ADC_REG_TRIG_EXT_TIM2_TRGO;
  ADC_REG_InitStruct.SequencerLength = LL_ADC_REG_SEQ_SCAN_DISABLE;
  ADC_REG_InitStruct.SequencerDiscont = LL_ADC_REG_SEQ_DISCONT_DISABLE;
  ADC_REG_InitStruct.ContinuousMode = LL_ADC_REG_CONV_SINGLE;
  ADC_REG_InitStruct.DMATransfer = LL_ADC_REG_DMA_TRANSFER_LIMITED;
  ADC_REG_InitStruct.Overrun = LL_ADC_REG_OVR_DATA_PRESERVED;
  LL_ADC_REG_Init(ADC1, &ADC_REG_InitStruct);
  LL_ADC_SetOverSamplingScope(ADC1, LL_ADC_OVS_DISABLE);

  // Init for both ADC
  Adc_PRV_InitCommon();

  for(uint8_t loc_u8idx = 0u; loc_u8idx < ADC_NB_SIGNALS; loc_u8idx++)
  {
    LL_ADC_REG_SetSequencerRanks(ADC1, LL_ADC_REG_RANK_1, arg_psSignal[loc_u8idx].Channel);
    LL_ADC_SetChannelSamplingTime(ADC1, arg_psSignal[loc_u8idx].Channel, LL_ADC_SAMPLINGTIME_47CYCLES_5);
    LL_ADC_SetChannelSingleDiff(ADC1, arg_psSignal[loc_u8idx].Channel, LL_ADC_SINGLE_ENDED);

    gbl_asAdcSignals[loc_u8idx] = arg_psSignal[loc_u8idx];
  }

  LL_ADC_EnableIT_OVR(ADC1);

  Adc_PRV_InitSamplingTimer();
}

/**
  @brief Start ADC acquisitions.
 */
void Adc_Activate(void)
{
  if (LL_ADC_IsEnabled(ADC1) == 0)
  {
    /* Disable ADC deep power down (enabled by default after reset state) */
    LL_ADC_DisableDeepPowerDown(ADC1);
    
    /* Enable ADC internal voltage regulator */
    LL_ADC_EnableInternalRegulator(ADC1);
    
    /* Delay for ADC internal voltage regulator stabilization.                */
    /* Compute number of CPU cycles to wait for, from delay in us.            */
    /* Note: Variable divided by 2 to compensate partially                    */
    /*       CPU processing cycles (depends on compilation optimization).     */
    /* Note: If system core clock frequency is below 200kHz, wait time        */
    /*       is only a few CPU processing cycles.                             */
    cpu_delay_WaitFor(LL_ADC_DELAY_INTERNAL_REGUL_STAB_US);
    
    /* Run ADC self calibration */
    LL_ADC_StartCalibration(ADC1, LL_ADC_SINGLE_ENDED);
    
    while (LL_ADC_IsCalibrationOnGoing(ADC1) != 0)
    {
    }
    
    /* Delay between ADC end of calibration and ADC enable.                   */
    /* Note: Variable divided by 2 to compensate partially                    */
    /*       CPU processing cycles (depends on compilation optimization).     */
    cpu_delay_WaitFor(ADC_DELAY_CALIB_ENABLE_CPU_CYCLES);
    
    /* Enable ADC */
    LL_ADC_Enable(ADC1);
    
    while (LL_ADC_IsActiveFlag_ADRDY(ADC1) == 0)
    {
    }
  }
}

/**
  @brief Return last value of acquired signal.
 */
uint32_t Adc_Get(ADCSignal_t arg_eSignal)
{
  uint32_t loc_u32Ret = 0u;
  if(arg_eSignal >= ADC_NB_SIGNALS)
  {
    Error_Handler();
  }
  else
  {
    loc_u32Ret = gbl_au32AdcValues[arg_eSignal];
  }
  return loc_u32Ret;
}

/**
  @brief Notify listeners at end of conversion.
 */
void Adc_Notify(void)
{
  for(uint8_t loc_u8idx = 0u; loc_u8idx < ADC_NB_SIGNALS; loc_u8idx++)
  {
    if(gbl_asAdcSignals[loc_u8idx].AcquisitionCallback != NULL)
    {
      gbl_asAdcSignals[loc_u8idx].AcquisitionCallback(gbl_au32AdcValues[loc_u8idx]);
    }
  }
}

/**
  @brief Setup the ADC with automatic DMA transfer.
 */
void Adc_Attach(ADCSignal_t arg_eSignal, fpCallbackADC arg_fpListener)
{
  if(arg_fpListener != NULL)
  {
    gbl_asAdcSignals[arg_eSignal].AcquisitionCallback = arg_fpListener;
  }
} 

void Adc_PRV_InitDMA(void)
{
  LL_AHB1_GRP1_EnableClock(LL_AHB1_GRP1_PERIPH_DMAMUX1);
  LL_AHB1_GRP1_EnableClock(LL_AHB1_GRP1_PERIPH_DMA1);

  NVIC_SetPriority(DMA1_Channel1_IRQn, 0);
  NVIC_EnableIRQ(DMA1_Channel1_IRQn);

  LL_DMA_SetPeriphRequest(DMA1, LL_DMA_CHANNEL_1, LL_DMAMUX_REQ_ADC1);

  LL_DMA_SetDataTransferDirection(DMA1, LL_DMA_CHANNEL_1, LL_DMA_DIRECTION_PERIPH_TO_MEMORY);

  LL_DMA_SetChannelPriorityLevel(DMA1, LL_DMA_CHANNEL_1, LL_DMA_PRIORITY_LOW);

  LL_DMA_SetMode(DMA1, LL_DMA_CHANNEL_1, LL_DMA_MODE_NORMAL);

  LL_DMA_SetPeriphIncMode(DMA1, LL_DMA_CHANNEL_1, LL_DMA_PERIPH_NOINCREMENT);

  LL_DMA_SetMemoryIncMode(DMA1, LL_DMA_CHANNEL_1, LL_DMA_MEMORY_INCREMENT);

  LL_DMA_SetPeriphSize(DMA1, LL_DMA_CHANNEL_1, LL_DMA_PDATAALIGN_HALFWORD);

  LL_DMA_SetMemorySize(DMA1, LL_DMA_CHANNEL_1, LL_DMA_MDATAALIGN_HALFWORD);

  LL_DMA_DisableChannelPrivilege(DMA1, LL_DMA_CHANNEL_1);

  LL_DMA_SetPeriphRequest(DMA1, LL_DMA_CHANNEL_1, LL_DMAMUX_REQ_ADC1);

  LL_DMA_SetDataTransferDirection(DMA1, LL_DMA_CHANNEL_1, LL_DMA_DIRECTION_PERIPH_TO_MEMORY);

  LL_DMA_SetChannelPriorityLevel(DMA1, LL_DMA_CHANNEL_1, LL_DMA_PRIORITY_LOW);

  LL_DMA_SetMode(DMA1, LL_DMA_CHANNEL_1, LL_DMA_MODE_NORMAL);

  LL_DMA_SetPeriphIncMode(DMA1, LL_DMA_CHANNEL_1, LL_DMA_PERIPH_NOINCREMENT);

  LL_DMA_SetMemoryIncMode(DMA1, LL_DMA_CHANNEL_1, LL_DMA_MEMORY_INCREMENT);

  LL_DMA_SetPeriphSize(DMA1, LL_DMA_CHANNEL_1, LL_DMA_PDATAALIGN_HALFWORD);

  LL_DMA_SetMemorySize(DMA1, LL_DMA_CHANNEL_1, LL_DMA_MDATAALIGN_HALFWORD);

  LL_DMA_DisableChannelPrivilege(DMA1, LL_DMA_CHANNEL_1);

  LL_DMA_SetPeriphRequest(DMA1, 
                          LL_DMA_CHANNEL_1,
                          LL_DMAMUX_REQ_ADC1);
  
  LL_DMA_ConfigAddresses(DMA1,
                         LL_DMA_CHANNEL_1,
                         LL_ADC_DMA_GetRegAddr(ADC1, LL_ADC_DMA_REG_REGULAR_DATA),
                         (uint32_t)gbl_au32AdcValues,
                         LL_DMA_DIRECTION_PERIPH_TO_MEMORY);
  
  LL_DMA_SetDataLength(DMA1,
                       LL_DMA_CHANNEL_1,
                       ADC_NB_SIGNALS);
  
  LL_DMA_EnableIT_TC(DMA1,
                     LL_DMA_CHANNEL_1);
  
  
  LL_DMA_EnableIT_TE(DMA1,
                     LL_DMA_CHANNEL_1);
  
  LL_DMA_EnableChannel(DMA1,
                       LL_DMA_CHANNEL_1);
}

void Adc_PRV_InitCommon(void)
{
  LL_ADC_CommonInitTypeDef ADC_CommonInitStruct = {0};

  ADC_CommonInitStruct.CommonClock = LL_ADC_CLOCK_SYNC_PCLK_DIV2;
  ADC_CommonInitStruct.Multimode = LL_ADC_MULTI_INDEPENDENT;
  LL_ADC_CommonInit(__LL_ADC_COMMON_INSTANCE(ADC1), &ADC_CommonInitStruct);
  LL_ADC_REG_SetTriggerEdge(ADC1, LL_ADC_REG_TRIG_EXT_RISING);

  LL_ADC_DisableDeepPowerDown(ADC1);
  LL_ADC_EnableInternalRegulator(ADC1);
  /* Delay for ADC internal voltage regulator stabilization. */
  /* Compute number of CPU cycles to wait for, from delay in us. */
  /* Note: Variable divided by 2 to compensate partially */
  /* CPU processing cycles (depends on compilation optimization). */
  /* Note: If system core clock frequency is below 200kHz, wait time */
  /* is only a few CPU processing cycles. */
  cpu_delay_WaitFor(LL_ADC_DELAY_INTERNAL_REGUL_STAB_US);
}

void Adc_PRV_InitSamplingTimer(void)
{
  uint32_t timer_clock_frequency = 0;             /* Timer clock frequency */
  uint32_t timer_prescaler = 0;                   /* Time base prescaler to have timebase aligned on minimum frequency possible */
  uint32_t timer_reload = 0;                      /* Timer reload value in function of timer prescaler to achieve time base period */

  LL_TIM_InitTypeDef TIM_InitStruct = {0};

  /* Peripheral clock enable */
  LL_APB1_GRP1_EnableClock(LL_APB1_GRP1_PERIPH_TIM2);

  /*## Configuration of NVIC #################################################*/
  /* Note: In this example, timer interrupts are not activated.               */
  /*       If needed, timer interruption at each time base period is          */
  /*       possible.                                                          */
  /*       Refer to timer examples.                                           */
  /* Configuration of timer as time base:                                     */ 
  /* Caution: Computation of frequency is done for a timer instance on APB1   */
  /*          (clocked by PCLK1)                                              */
  /* Timer frequency is configured from the following constants:              */
  /* - TIMER_FREQUENCY: timer frequency (unit: Hz).                           */
  /* - TIMER_FREQUENCY_RANGE_MIN: timer minimum frequency possible            */
  /*   (unit: Hz).                                                            */
  /* Note: Refer to comments at these literals definition for more details.   */
  
  /* Retrieve timer clock source frequency */
  /* If APB1 prescaler is different of 1, timers have a factor x2 on their    */
  /* clock source.                                                            */
  if (LL_RCC_GetAPB1Prescaler() == LL_RCC_APB1_DIV_1)
  {
    timer_clock_frequency = __LL_RCC_CALC_PCLK1_FREQ(SystemCoreClock, LL_RCC_GetAPB1Prescaler());
  }
  else
  {
    timer_clock_frequency = (__LL_RCC_CALC_PCLK1_FREQ(SystemCoreClock, LL_RCC_GetAPB1Prescaler()) * 2);
  }
  
  /* Timer prescaler calculation */
  /* (computation for timer 16 bits, additional + 1 to round the prescaler up) */
  timer_prescaler = ((timer_clock_frequency / (TIMER_PRESCALER_MAX_VALUE * TIMER_FREQUENCY_RANGE_MIN)) +1);
  /* Timer reload calculation */
  timer_reload = (timer_clock_frequency / (timer_prescaler * TIMER_FREQUENCY));
  TIM_InitStruct.Prescaler = (timer_prescaler - 1);
  TIM_InitStruct.CounterMode = LL_TIM_COUNTERMODE_UP;
  TIM_InitStruct.Autoreload = (timer_reload - 1);
  TIM_InitStruct.ClockDivision = LL_TIM_CLOCKDIVISION_DIV1;
  LL_TIM_Init(TIM2, &TIM_InitStruct);
  LL_TIM_DisableARRPreload(TIM2);
  LL_TIM_SetTriggerInput(TIM2, LL_TIM_TS_ITR0);
  LL_TIM_SetSlaveMode(TIM2, LL_TIM_SLAVEMODE_DISABLED);
  LL_TIM_DisableIT_TRIG(TIM2);
  LL_TIM_DisableDMAReq_TRIG(TIM2);
  LL_TIM_SetTriggerOutput(TIM2, LL_TIM_TRGO_UPDATE);
  LL_TIM_DisableMasterSlaveMode(TIM2);
  /* Set timer the trigger output (TRGO) */
  LL_TIM_SetTriggerOutput(TIM2, LL_TIM_TRGO_UPDATE);  
  /* Enable counter */
  LL_TIM_EnableCounter(TIM2);
}