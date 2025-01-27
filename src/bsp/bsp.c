#include <stddef.h>

#include "bsp.h"

#include "system.h"
#include "adc.h"
#include "adc_config.h"
#include "gpio.h"
#include "board.h"
#include "timer_counter.h"
#include "timer_pwm.h"
#include "uart.h"
#include "watchdog.h"

const SignalConfig_t gbl_sAdcSignalConfig[ADC_NB_SIGNALS] =
{
  {
    BOARD_ADC_MTR_CURRENT_CHANNEL,
    NULL
  }
};

/**
  @brief Setup the BSP.
 */
void Bsp_Init (void)
{
  system_InitSystem();

  Gpio_Init();

  Adc_Init(gbl_sAdcSignalConfig);
  TimerCounter_Init();
  TimerPwm_Init();
  Uart_Init();
}

/**
  @brief Activate the BSP.
 */
void Bsp_Activate (void)
{
  Adc_Activate();
  TimerCounter_Activate();
  TimerPwm_Activate();
  Uart_Activate();
  Watchdog_Activate(); // Last to be activated, to not trigger reset during Init
}
