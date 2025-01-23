#ifndef _INTERRUPTS_H_
#define _INTERRUPTS_H_

typedef enum
{
  INT_ADC,
  INT_DMA_CH1,
  INT_DMA_CH2,
  INT_TIMER_COUNTER,
  INT_TIMER_PWM,
  INT_UART,
  INT_WDOG,
  INT_LPTIM,
  INT_MAX
} Interrupts_Desc_t;

/**
  @brief Setup given interrupt via NVIC.
 */
void Interrupts_Enable(Interrupts_Desc_t arg_eInterrupt);

#endif // _INTERRUPTS_H_