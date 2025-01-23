#ifndef _SYSTEM_CONFIG_H_
#define _SYSTEM_CONFIG_H_

#if !defined  (HSE_VALUE)
#define HSE_VALUE          16000000UL /*!< Value of the External oscillator in Hz */
#endif /* HSE_VALUE */

#if !defined  (MSI_VALUE)
#define MSI_VALUE          4000000UL /*!< Value of the Internal oscillator in Hz*/
#endif /* MSI_VALUE */

#if !defined  (HSI_VALUE)
#define HSI_VALUE          16000000UL /*!< Value of the Internal oscillator in Hz*/
#endif /* HSI_VALUE */

#if !defined  (HSI48_VALUE)
#define HSI48_VALUE        48000000UL /*!< Value of the Internal High Speed oscillator for USB FS/SDMMC/RNG in Hz.
                                          The real value my vary depending on manufacturing process variations.*/
#endif /* HSI48_VALUE */

#if !defined  (LSI_VALUE)
#define LSI_VALUE           32000UL    /*!< LSI Typical Value in Hz*/
#endif /* LSI_VALUE */                  /*!< Value of the Internal Low Speed oscillator in Hz

#if !defined  (LSE_VALUE)
#define LSE_VALUE          32768UL    /*!< Value of the External oscillator in Hz*/
#endif /* LSE_VALUE */

#if !defined  (EXTERNAL_SAI1_CLOCK_VALUE)
#define EXTERNAL_SAI1_CLOCK_VALUE  48000UL /*!< Value of the SAI1 External clock source in Hz*/
#endif /* EXTERNAL_SAI1_CLOCK_VALUE */

#if !defined  (EXTERNAL_SAI2_CLOCK_VALUE)
#define EXTERNAL_SAI2_CLOCK_VALUE   48000UL /*!< Value of the SAI2 External clock source in Hz*/
#endif /* EXTERNAL_SAI2_CLOCK_VALUE */

#endif // _SYSTEM_CONFIG_H_