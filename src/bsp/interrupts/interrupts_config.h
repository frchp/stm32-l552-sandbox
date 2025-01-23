#ifndef _INTERRUPTS_CONFIG_H_
#define _INTERRUPTS_CONFIG_H_

#include <stdint.h>
#include "interrupts.h"
#include "stm32l552xx.h"
#include "FreeRTOSConfig.h"

typedef struct
{
  Interrupts_Desc_t ApplicationName;
  IRQn_Type NVIC_IRQ;
  uint32_t Priority;
} Interrupts_Config_t;

#define NB_INTERRUPTS_USED      5u
#define BASE_INTERRUPT_PRIORITY (configMAX_SYSCALL_INTERRUPT_PRIORITY - 1)

const Interrupts_Config_t gbl_InterruptsArray[NB_INTERRUPTS_USED] =
{
  {INT_ADC, ADC1_2_IRQn, BASE_INTERRUPT_PRIORITY},
  {INT_UART, LPUART1_IRQn, BASE_INTERRUPT_PRIORITY},
  {INT_DMA_CH2, DMA1_Channel2_IRQn, BASE_INTERRUPT_PRIORITY}, /* DMA for Timer counter */
  {INT_DMA_CH1, DMA1_Channel1_IRQn, BASE_INTERRUPT_PRIORITY}, /* DMA for ADC */
  {INT_LPTIM, LPTIM1_IRQn, configLIBRARY_LOWEST_INTERRUPT_PRIORITY}
};

#endif // _INTERRUPTS_CONFIG_H_