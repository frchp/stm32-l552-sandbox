#include <stddef.h>
#include <assert.h>
#include <stdbool.h>

#include "adc.h"
#include "adc_config.h"

#include "cpu_delay.h"
#include "error.h"
#include "interrupts.h"

#include "stm32l5xx_ll_dma.h"
#include "stm32l5xx_ll_tim.h"
#include "stm32l5xx_ll_rcc.h"
#include "stm32l5xx_ll_bus.h"
#include "stm32l5xx_ll_adc.h"

static uint16_t gbl_au16AdcValues[ADC_NB_SIGNALS];
static SignalConfig_t gbl_asAdcSignals[ADC_NB_SIGNALS];

#define ADC_NB_RANKS (16u)
static const uint32_t gbl_cau32AdcRankCfg[ADC_NB_RANKS] = {
  LL_ADC_REG_RANK_1,
  LL_ADC_REG_RANK_2,
  LL_ADC_REG_RANK_3,
  LL_ADC_REG_RANK_4,
  LL_ADC_REG_RANK_5,
  LL_ADC_REG_RANK_6,
  LL_ADC_REG_RANK_7,
  LL_ADC_REG_RANK_8,
  LL_ADC_REG_RANK_9,
  LL_ADC_REG_RANK_10,
  LL_ADC_REG_RANK_11,
  LL_ADC_REG_RANK_12,
  LL_ADC_REG_RANK_13,
  LL_ADC_REG_RANK_14,
  LL_ADC_REG_RANK_15,
  LL_ADC_REG_RANK_16
};

#define ADC_DELAY_CALIB_ENABLE_CPU_CYCLES  (LL_ADC_DELAY_CALIB_ENABLE_ADC_CYCLES * 32u)
#define TIMER_FREQUENCY (200u)
#define TIMER_FREQUENCY_RANGE_MIN      (1UL)
#define TIMER_PRESCALER_MAX_VALUE      (0xFFFF - 1UL)

static void Adc_PRV_InitDMA(void);
static void Adc_PRV_InitSamplingTimer(void);

/**
  @brief Setup the ADC with automatic DMA transfer.
 */
void Adc_Init(const SignalConfig_t* arg_psSignal)
{
  static bool bInitialized = false;
  if(!bInitialized)
  {
    bInitialized = true;

    LL_RCC_SetADCClockSource(LL_RCC_ADC_CLKSOURCE_SYSCLK);

    LL_AHB2_GRP1_EnableClock(LL_AHB2_GRP1_PERIPH_ADC);

    Adc_PRV_InitDMA();

    Interrupts_Enable(INT_ADC);

    // Configure ADC1
    LL_ADC_SetResolution(ADC1, LL_ADC_RESOLUTION_12B);
    LL_ADC_SetDataAlignment(ADC1, LL_ADC_DATA_ALIGN_RIGHT);
    LL_ADC_SetLowPowerMode(ADC1, LL_ADC_LP_MODE_NONE);

    // Configure ADC1 external trigger source and polarity
    LL_ADC_REG_SetTriggerSource(ADC1, LL_ADC_REG_TRIG_EXT_TIM2_TRGO);
    LL_ADC_REG_SetTriggerEdge(ADC1, LL_ADC_REG_TRIG_EXT_RISING);

    // Enable continuous mode and DMA
    LL_ADC_REG_SetContinuousMode(ADC1, LL_ADC_REG_CONV_SINGLE);
    LL_ADC_REG_SetDMATransfer(ADC1, LL_ADC_REG_DMA_TRANSFER_UNLIMITED);

    LL_ADC_DisableDeepPowerDown(ADC1);
    LL_ADC_EnableInternalRegulator(ADC1);

    assert(ADC_NB_SIGNALS < ADC_NB_RANKS);
    for(uint8_t loc_u8idx = 0u; loc_u8idx < ADC_NB_SIGNALS; loc_u8idx++)
    {
      LL_ADC_REG_SetSequencerRanks(ADC1, gbl_cau32AdcRankCfg[loc_u8idx], arg_psSignal[loc_u8idx].Channel);
      LL_ADC_SetChannelSamplingTime(ADC1, arg_psSignal[loc_u8idx].Channel, LL_ADC_SAMPLINGTIME_47CYCLES_5);
      LL_ADC_SetChannelSingleDiff(ADC1, arg_psSignal[loc_u8idx].Channel, LL_ADC_SINGLE_ENDED);

      gbl_asAdcSignals[loc_u8idx] = arg_psSignal[loc_u8idx];
    }

    LL_ADC_EnableIT_OVR(ADC1);

    Adc_PRV_InitSamplingTimer();
  }
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

  if ((LL_ADC_IsEnabled(ADC1) == 1)               &&
      (LL_ADC_IsDisableOngoing(ADC1) == 0)        &&
      (LL_ADC_REG_IsConversionOngoing(ADC1) == 0)   )
  {
    LL_ADC_REG_StartConversion(ADC1);
  }
}

/**
  @brief Return last value of acquired signal.
 */
uint16_t Adc_Get(ADCSignal_t arg_eSignal)
{
  uint16_t loc_u16Ret = 0u;
  if(arg_eSignal >= ADC_NB_SIGNALS)
  {
    Error_Handler();
  }
  else
  {
    loc_u16Ret = gbl_au16AdcValues[arg_eSignal];
  }
  return loc_u16Ret;
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
      gbl_asAdcSignals[loc_u8idx].AcquisitionCallback(gbl_au16AdcValues[loc_u8idx]);
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

static void Adc_PRV_InitDMA(void)
{
  LL_AHB1_GRP1_EnableClock(LL_AHB1_GRP1_PERIPH_DMAMUX1);
  LL_AHB1_GRP1_EnableClock(LL_AHB1_GRP1_PERIPH_DMA1);

  Interrupts_Enable(INT_DMA_CH1);

  LL_DMA_SetPeriphRequest(DMA1, LL_DMA_CHANNEL_1, LL_DMAMUX_REQ_ADC1);
  LL_DMA_SetDataTransferDirection(DMA1, LL_DMA_CHANNEL_1, LL_DMA_DIRECTION_PERIPH_TO_MEMORY);
  LL_DMA_SetChannelPriorityLevel(DMA1, LL_DMA_CHANNEL_1, LL_DMA_PRIORITY_LOW);
  LL_DMA_SetMode(DMA1, LL_DMA_CHANNEL_1, LL_DMA_MODE_CIRCULAR);
  LL_DMA_SetPeriphIncMode(DMA1, LL_DMA_CHANNEL_1, LL_DMA_PERIPH_NOINCREMENT);
  LL_DMA_SetMemoryIncMode(DMA1, LL_DMA_CHANNEL_1, LL_DMA_MEMORY_INCREMENT);
  LL_DMA_SetPeriphSize(DMA1, LL_DMA_CHANNEL_1, LL_DMA_MDATAALIGN_HALFWORD);
  LL_DMA_SetMemorySize(DMA1, LL_DMA_CHANNEL_1, LL_DMA_MDATAALIGN_HALFWORD);
  LL_DMA_DisableChannelPrivilege(DMA1, LL_DMA_CHANNEL_1);
  LL_DMA_SetPeriphRequest(DMA1, 
                          LL_DMA_CHANNEL_1,
                          LL_DMAMUX_REQ_ADC1);
  LL_DMA_ConfigAddresses(DMA1,
                         LL_DMA_CHANNEL_1,
                         LL_ADC_DMA_GetRegAddr(ADC1, LL_ADC_DMA_REG_REGULAR_DATA),
                         (uint32_t)gbl_au16AdcValues,
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

static void Adc_PRV_InitSamplingTimer(void)
{
  uint32_t timer_clock_frequency = 0;             /* Timer clock frequency */
  uint32_t timer_prescaler = 0;                   /* Time base prescaler to have timebase aligned on minimum frequency possible */
  uint32_t timer_reload = 0;                      /* Timer reload value in function of timer prescaler to achieve time base period */

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
  LL_TIM_SetClockDivision(TIM2, LL_TIM_CLOCKDIVISION_DIV4);
  LL_TIM_SetPrescaler(TIM2, (timer_prescaler - 1));
  LL_TIM_SetAutoReload(TIM2, (timer_reload - 1));
  LL_TIM_SetCounterMode(TIM2, LL_TIM_COUNTERMODE_UP); 
  LL_TIM_SetRepetitionCounter(TIM2, 0);
  LL_TIM_SetSlaveMode(TIM2, LL_TIM_SLAVEMODE_DISABLED);
  LL_TIM_SetTriggerOutput(TIM2, LL_TIM_TRGO_UPDATE);
  LL_TIM_DisableMasterSlaveMode(TIM2);
  LL_TIM_EnableCounter(TIM2);
  LL_TIM_GenerateEvent_UPDATE(TIM2);
}