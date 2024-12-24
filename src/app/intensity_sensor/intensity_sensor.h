#ifndef _INTENSITY_SENSOR_H_
#define _INTENSITY_SENSOR_H_

#include <stdint.h>

/**
  @brief Setup the IntensitySensor.
 */
void IntensitySensor_Init(void);

/**
  @brief Update IntensitySensor.
 */
void IntensitySensor_Update(uint32_t arg_u32NewValueLSB);

/**
  @brief Return last current.
 */
uint32_t IntensitySensor_Get(void);

#endif // _INTENSITY_SENSOR_H_