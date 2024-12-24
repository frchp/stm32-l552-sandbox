#ifndef _ADC_H_
#define _ADC_H_

#include <stdint.h>

typedef void (*fpCallbackADC)(uint32_t);

typedef struct
{
  uint32_t Channel;
  fpCallbackADC AcquisitionCallback;
} SignalConfig_t;

typedef enum
{
  ADC_SIG_CURRENT
} ADCSignal_t;

/**
  @brief Setup the ADC with automatic DMA transfer.
 */
void Adc_Init(const SignalConfig_t* arg_psSignal);

/**
  @brief Start ADC acquisitions.
 */
void Adc_Activate(void);

/**
  @brief Return last value of acquired signal.
 */
uint16_t Adc_Get(ADCSignal_t arg_eSignal);

/**
  @brief Notify listeners at end of conversion.
 */
void Adc_Notify(void);

/**
  @brief Setup the ADC with automatic DMA transfer.
 */
void Adc_Attach(ADCSignal_t arg_eSignal, fpCallbackADC arg_fpListener);

#endif // _ADC_H_