#ifndef _IDLE_TASK_H_
#define _IDLE_TASK_H_

#include <stdint.h>

/*
* @brief Is called by FreeRTOS before entering sleep.
*/
void Idle_OnPreSleepProcessing(uint32_t xExpectedTime);

/*
* @brief Is called by FreeRTOS after entering sleep.
*/
void Idle_OnPostSleepProcessing(uint32_t xExpectedTime);

/*
* @brief Is called by FreeRTOS during Idle if user defined tickless idle should be implemented.
*/
void Idle_TicklessIdleSleep(uint32_t xExpectedTime);

#endif // _IDLE_TASK_H_
