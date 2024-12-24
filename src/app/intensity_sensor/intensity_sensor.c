#include "intensity_sensor.h"
#include "intensity_sensor_config.h"
#include "adc.h"
#include "stm32l5xx_ll_adc.h"

#define THOUSAND_FACTOR (1000u)

static uint32_t gbl_u32LastCurrentInLSB = 0u;

/**
  @brief Setup the IntensitySensor.
 */
void IntensitySensor_Init(void)
{
  Adc_Attach(ADC_SIG_CURRENT, IntensitySensor_Update);
}

/**
  @brief Update IntensitySensor.
 */
void IntensitySensor_Update(uint32_t arg_u32NewValueLSB)
{
  gbl_u32LastCurrentInLSB = arg_u32NewValueLSB;
}

/**
  @brief Return last current.
 */
uint32_t IntensitySensor_Get(void)
{
  uint32_t loc_u32CurrentInMillivolt = __LL_ADC_CALC_DATA_TO_VOLTAGE(INTENSITY_SENSOR_VDDA, gbl_u32LastCurrentInLSB, LL_ADC_RESOLUTION_12B);

  return (loc_u32CurrentInMillivolt * THOUSAND_FACTOR)/(INTENSITY_SENSOR_SHUNT_IN_MILLIOHMS * INTENSITY_SENSOR_GAIN);
}
