#include <stdlib.h>

#include "speed_sensor.h"
#include "speed_sensor_config.h"

#include "timer_counter.h"

static uint32_t gbl_u32PulsesDelta = 0u;

#define RATIO_MIN_TO_SEC (60u)

/**
  @brief Setup the SpeedSensor.
 */
void SpeedSensor_Init(void)
{
  TimerCounter_Attach(SpeedSensor_Update);
}

/**
  @brief Update SpeedSensor when measurement is done.
 */
void SpeedSensor_Update(uint32_t* arg_au32CountBuffer, uint8_t arg_u8Size)
{
  gbl_u32PulsesDelta = abs((int32_t)(arg_au32CountBuffer[arg_u8Size] - arg_au32CountBuffer[arg_u8Size - 1]));
}

/**
  @brief Return last speed in rpm.
 */
uint32_t SpeedSensor_Get(void)
{
  // We know the time delta between two pulses, when 4 are required to do a revolution
  return ((gbl_u32PulsesDelta * SPEED_SENSOR_PULSES_PER_REV * RATIO_MIN_TO_SEC)/TimerCounter_GetTimerFrequency());
}