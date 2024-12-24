#ifndef _TIMER_PWM_H_
#define _TIMER_PWM_H_

#include <stdint.h>

/**
  @brief Setup the TimerPwm.
 */
void TimerPwm_Init(void);

/**
  @brief Start TimerPwm.
 */
void TimerPwm_Activate(void);

/**
  @brief Set duty cycle.
 */
void TimerPwm_SetDutyCycle(uint32_t arg_u32DutyCycle);

#endif // _TIMER_PWM_H_