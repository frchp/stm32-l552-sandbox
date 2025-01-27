#include "error.h"

typedef struct
{
  bool isCritical;
  Error_Component_t eComponent;
  Error_Type_t eType;
} Error_t;


static Error_t gbl_sLastRecordedError __attribute__(( section(".noinit") ));

/**
 * @brief Handle incoming error
 */
void Error_Handler(bool arg_bIsCritical, Error_Component_t arg_eComponent, Error_Type_t arg_eType)
{
  gbl_sLastRecordedError.isCritical = arg_bIsCritical;
  gbl_sLastRecordedError.eComponent = arg_eComponent;
  gbl_sLastRecordedError.eType = arg_eType;

  if(arg_bIsCritical)
  {
    // Wait until watchdog reset
    while(1);
  }
  else
  {
    // Save error and let program live
    return;
  }
}