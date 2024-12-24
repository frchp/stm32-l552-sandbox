#include "stm32l5xx_ll_dma.h"
#include "stm32l5xx_ll_tim.h"
#include "stm32l5xx_ll_adc.h"
#include "stm32l5xx_ll_lpuart.h"

#include "error.h"
#include "adc.h"
#include "timer_counter.h"
#include "timer_pwm.h"
#include "uart.h"

/**
  * @brief This function handles System tick timer.
  */
void SysTick_Handler(void)
{
}

/**
  * @brief This function handles Hard fault interrupt.
  */
void HardFault_Handler(void)
{
  Error_Handler();
}

/**
  * @brief This function handles Memory management fault.
  */
void MemManage_Handler(void)
{
  Error_Handler();
}

/**
  * @brief This function handles Prefetch fault, memory access fault.
  */
void BusFault_Handler(void)
{
  Error_Handler();
}

/**
  * @brief This function handles Undefined instruction or illegal state.
  */
void UsageFault_Handler(void)
{
  Error_Handler();
}

/**
  * @brief  This function handles ADC1 interrupt request.
  */
void ADC1_2_IRQHandler(void)
{
  /* Check whether ADC group regular overrun caused the ADC interruption */
  if(LL_ADC_IsActiveFlag_OVR(ADC1))
  {
    /* Clear flag ADC group regular overrun */
    LL_ADC_ClearFlag_OVR(ADC1);
    
    LL_ADC_DisableIT_OVR(ADC1);
  }
}

/**
  * @brief This function handles DMA1 channel1 global interrupt.
  */
void DMA1_Channel1_IRQHandler(void)
{
  /* Check whether DMA transfer complete caused the DMA interruption */
  if(LL_DMA_IsActiveFlag_TC1(DMA1))
  {
    /* Clear flag DMA transfer complete */
    LL_DMA_ClearFlag_TC1(DMA1);
    
    /* Call interruption treatment function */
    Adc_Notify();
  }
  
  /* Check whether DMA transfer error caused the DMA interruption */
  if(LL_DMA_IsActiveFlag_TE1(DMA1))
  {
    /* Clear flag DMA transfer error */
    LL_DMA_ClearFlag_TE1(DMA1);
    
    Error_Handler();
  }
}

void LPUART1_IRQHandler(void)
{
  /* Check RXNE flag value in ISR register */
  if (LL_LPUART_IsActiveFlag_RXNE(LPUART1))
  {
    /* RXNE flag will be cleared by reading of RDR register (done in call) */
    /* Call function in charge of handling Character reception */
    Uart_RxByteComplete();
  }

  if(LL_LPUART_IsActiveFlag_NE(LPUART1))
  {
    /* case Noise Error flag is raised : Clear NF Flag */
    LL_LPUART_ClearFlag_NE(LPUART1);
  }
}

void DMA1_Channel2_IRQHandler(void)
{
  if (LL_DMA_IsActiveFlag_TC2(DMA1))
  {
    LL_DMA_ClearFlag_TC2(DMA1); // Clear transfer complete flag

    TimerCounter_AllPulsesDetected();
  }
}