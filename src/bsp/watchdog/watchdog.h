#ifndef _WATCHDOG_H_
#define _WATCHDOG_H_

/**
  @brief Setup the Watchdog.
 */
void Watchdog_Activate(void);

/**
  @brief Kick the dog.
 */
void Watchdog_Refresh(void);

#endif // _WATCHDOG_H_