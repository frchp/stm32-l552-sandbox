#ifndef _SPEED_SENSOR_H_
#define _SPEED_SENSOR_H_

#include <stdint.h>

/**
  @brief Setup the SpeedSensor.
 */
void SpeedSensor_Init(void);

/**
  @brief Update SpeedSensor.
 */
void SpeedSensor_Update(uint32_t* arg_au32CountBuffer, uint8_t arg_u8Size);

/**
  @brief Return last speed in rpm.
 */
uint32_t SpeedSensor_Get(void);

#endif // _SPEED_SENSOR_H_