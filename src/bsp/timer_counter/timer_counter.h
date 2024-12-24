#ifndef _TIMER_COUNTER_H_
#define _TIMER_COUNTER_H_

#include <stdint.h>

// Gives buffer and size of buffer
typedef void (*fpCounterCallback)(uint32_t*, uint8_t);

/**
  @brief Setup the TimerCounter.
 */
void TimerCounter_Init(void);

/**
  @brief Start TimerCounter.
 */
void TimerCounter_Activate(void);

/**
  @brief Return TimerCounter frequency.
 */
uint32_t TimerCounter_GetTimerFrequency(void);

/**
  @brief All pulses are detected, notify listeners.
 */
void TimerCounter_AllPulsesDetected(void);

/**
  @brief Attach listeners.
 */
void TimerCounter_Attach(fpCounterCallback arg_fpListener);


#endif // _TIMER_COUNTER_H_