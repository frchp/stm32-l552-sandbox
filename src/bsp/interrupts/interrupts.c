#include <assert.h>
#include <stdbool.h>

#include "interrupts.h"
#include "interrupts_config.h"

#include "stm32l5xx_ll_dma.h"
#include "stm32l5xx_ll_tim.h"
#include "stm32l5xx_ll_adc.h"
#include "stm32l5xx_ll_lpuart.h"
#include "stm32l5xx_ll_lptim.h"

#include "idle_task.h"
#include "error.h"
#include "adc.h"
#include "timer_counter.h"
#include "timer_pwm.h"
#include "uart.h"

/**
  @brief Setup given interrupt via NVIC.
 */
void Interrupts_Enable(Interrupts_Desc_t arg_eInterrupt)
{
  bool loc_bCfgFound = false;
  Interrupts_Config_t loc_sPeripheralCfg;
  assert(arg_eInterrupt < INT_MAX);

  for(uint8_t loc_u8Idx = 0u; (loc_u8Idx < NB_INTERRUPTS_USED) && (loc_bCfgFound == false); loc_u8Idx++)
  {
    if(gbl_InterruptsArray[loc_u8Idx].ApplicationName == arg_eInterrupt)
    {
      loc_bCfgFound = true;
      loc_sPeripheralCfg = gbl_InterruptsArray[loc_u8Idx];
    }
  };

  if(loc_bCfgFound == true)
  {
    NVIC_SetPriority(loc_sPeripheralCfg.NVIC_IRQ, loc_sPeripheralCfg.Priority);
    NVIC_EnableIRQ(loc_sPeripheralCfg.NVIC_IRQ);
  }
}

/**
  * @brief This function handles Hard fault interrupt.
  */
void HardFault_Handler(void)
{
  Error_Handler(true, ERR_BSP_CORTEX_FAULT, ERR_TYPE_FAULT);
}

/**
  * @brief This function handles Memory management fault.
  */
void MemManage_Handler(void)
{
  Error_Handler(true, ERR_BSP_CORTEX_FAULT, ERR_TYPE_FAULT);
}

/**
  * @brief This function handles Prefetch fault, memory access fault.
  */
void BusFault_Handler(void)
{
  Error_Handler(true, ERR_BSP_CORTEX_FAULT, ERR_TYPE_FAULT);
}

/**
  * @brief This function handles Undefined instruction or illegal state.
  */
void UsageFault_Handler(void)
{
  Error_Handler(true, ERR_BSP_CORTEX_FAULT, ERR_TYPE_FAULT);
}

/**
  * @brief  This function handles ADC1 interrupt request.
  */
void ADC1_2_IRQHandler(void)
{
  if(LL_ADC_IsActiveFlag_OVR(ADC1))
  {
    LL_ADC_ClearFlag_OVR(ADC1);

    LL_ADC_DisableIT_OVR(ADC1);
  }
}

/**
  * @brief This function handles DMA1 channel1 global interrupt.
  */
void DMA1_Channel1_IRQHandler(void)
{
  if(LL_DMA_IsActiveFlag_TC1(DMA1))
  {
    LL_DMA_ClearFlag_TC1(DMA1);

    Adc_Notify();
  }

  if(LL_DMA_IsActiveFlag_TE1(DMA1))
  {
    LL_DMA_ClearFlag_TE1(DMA1);

    Error_Handler(false, ERR_BSP_DMA_CH1, ERR_TYPE_IRQ);
  }
}

void LPUART1_IRQHandler(void)
{
  if (LL_LPUART_IsActiveFlag_RXNE(LPUART1))
  {
    // Flag is cleared by reading register
    Uart_RxByteComplete();
  }

  if(LL_LPUART_IsActiveFlag_TC(LPUART1))
  {
    LL_LPUART_ClearFlag_TC(LPUART1);

    Uart_TxByteComplete();
  }

  if(LL_LPUART_IsActiveFlag_ORE(LPUART1)
   || LL_LPUART_IsActiveFlag_PE(LPUART1)
   || LL_LPUART_IsActiveFlag_FE(LPUART1))
  {
    LL_LPUART_ClearFlag_ORE(LPUART1);
    LL_LPUART_ClearFlag_PE(LPUART1);
    LL_LPUART_ClearFlag_FE(LPUART1);

    Error_Handler(false, ERR_BSP_UART, ERR_TYPE_IRQ);
  }
}

void DMA1_Channel2_IRQHandler(void)
{
  if (LL_DMA_IsActiveFlag_TC2(DMA1))
  {
    LL_DMA_ClearFlag_TC2(DMA1);

    TimerCounter_AllPulsesDetected();
  }

  if(LL_DMA_IsActiveFlag_TE2(DMA1))
  {
    LL_DMA_ClearFlag_TE2(DMA1);

    Error_Handler(false, ERR_BSP_DMA_CH2, ERR_TYPE_IRQ);
  }
}

void LPTIM1_IRQHandler(void)
{
  if (LL_LPTIM_IsActiveFlag_CMPM(LPTIM1))
  {
    LL_LPTIM_ClearFlag_CMPM(LPTIM1);
    // To wake up uC only
  }
}
