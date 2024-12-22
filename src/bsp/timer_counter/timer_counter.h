#ifndef _TIMER_COUNTER_H_
#define _TIMER_COUNTER_H_

#include <stdint.h>

/**
  @brief Setup the TimerCounter.
 */
void TimerCounter_Init(void);

/**
  @brief Start TimerCounter.
 */
void TimerCounter_Activate(void);

/**
  @brief Return last value of counted pulses.
 */
uint32_t TimerCounter_Get(void);

/**
  @brief Period has elapsed, compute count.
 */
void TimerCounter_TimeElapsed(void);


#endif // _TIMER_COUNTER_H_