#ifndef _SYSTEM_H_
#define _SYSTEM_H_

/**
  @brief Setup the microcontroller system.

   Initialize the System and update the SystemCoreClock variable.
 */
void SystemInit (void);

/**
  @brief Update clocks based on register value.
 */
void SystemCoreClockUpdate(void);

/**
  @brief Setup the clock tree and system.
 */
void system_InitSystem(void);

void system_InitGpio(void);

#endif // _SYSTEM_H_