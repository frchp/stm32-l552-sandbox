#include "supervisor_task.h"

#include "FreeRTOS.h"
#include "task.h"
#include "timers.h"

#include "error.h"
#include "motor_task.h"

/* Supervisor task parameters */
TaskHandle_t gbl_sSupervisorTaskHandle;
StackType_t gbl_sStackSupervisor [SUPERVISOR_TASK_STACK_SIZE];
StaticTask_t gbl_sTCBSupervisor;

#define SUPERVISOR_TIMER_PERIOD_MS (300u)
TimerHandle_t gbl_sSupervisorTimerHandle;
StaticTimer_t gbl_sSupervisorTimerBuff;

#define SUPERVISOR_TASK_ALIVE_MASK (0x01)
typedef union {
  uint8_t u8Value;  // Full byte access
  struct {
    uint8_t bitMotor : 1;
    uint8_t bit1 : 1;
    uint8_t bit2 : 1;
    uint8_t bit3 : 1;
    uint8_t bit4 : 1;
    uint8_t bit5 : 1;
    uint8_t bit6 : 1;
    uint8_t bit7 : 1;
  } sBit;  // Individual bit access
} Supervisor_Bitfield_t;

static bool gbl_bTaskInitialized = false;
static Supervisor_Bitfield_t gbl_sSupervisorState = {.u8Value = 0u};

static void SupervisorTask_TimerCallback(TimerHandle_t xTimer);
static void SupervisorTask_StartSupervision(void);

void SupervisorTask(void *arg_pvParameters)
{
  (void)arg_pvParameters; // Avoid compiler warning for unused parameter
  uint32_t u32Notifications = 0ul;
  uint32_t u32Status = pdPASS;

  gbl_sSupervisorTimerHandle = xTimerCreateStatic
                 ( "SupTimer",
                   pdMS_TO_TICKS(SUPERVISOR_TIMER_PERIOD_MS),
                   pdFALSE /* One shot timer restarted by user */,
                   ( void * ) 0,
                   SupervisorTask_TimerCallback,
                   &gbl_sSupervisorTimerBuff );
  if(gbl_sSupervisorTimerHandle == NULL)
  {
    Error_Handler(true, ERR_OS_SUPERVISOR_TASK, ERR_TYPE_INIT);
  }
  else
  {
    gbl_bTaskInitialized = true;
    SupervisorTask_StartSupervision();
  }

  for (;;)
  {
    u32Status = xTaskNotifyWait( pdFALSE,          /* Don't clear bits on entry. */
                                 UINT32_MAX,        /* Clear all bits on exit. */
                                 &u32Notifications, /* Stores the notified value. */
                                 portMAX_DELAY );

    if( u32Status == pdPASS )
    {
      if( ( u32Notifications & SUPERVISOR_EVT_MTRALIVE ) != 0 )
      {
        gbl_sSupervisorState.sBit.bitMotor = 1u;
      }

      if( ( u32Notifications & SUPERVISOR_EVT_TIMERELAPSED ) != 0 )
      {
        if((gbl_sSupervisorState.u8Value & SUPERVISOR_TASK_ALIVE_MASK) == 0u)
        {
          Error_Handler(true, ERR_OS_SUPERVISOR_TASK, ERR_TYPE_TIMEOUT);
        }

        // Reset of bit field
        gbl_sSupervisorState.u8Value = 0u;

        // Restart timer
        SupervisorTask_StartSupervision();
      }
    }
    else
    {
      // OS error if we are here
      Error_Handler(true, ERR_OS_MOTOR_TASK, ERR_TYPE_TIMEOUT);
    }
  }
}

/**
 * @brief Is called by Motor task to notify it is still alive.
 * @details Should only be called in task context
 */
void SupervisorTask_MotorTaskIsAlive(void)
{
  if(gbl_bTaskInitialized)
  {
    xTaskNotify( gbl_sSupervisorTaskHandle,
                        SUPERVISOR_EVT_MTRALIVE,
                        eSetBits );
  }
}

static void SupervisorTask_TimerCallback(TimerHandle_t xTimer)
{
  // Notify Supervisor task to check its bit count
  if(gbl_bTaskInitialized)
  {
    xTaskNotify( gbl_sSupervisorTaskHandle,
                  SUPERVISOR_EVT_TIMERELAPSED,
                  eSetBits );
  }
}

static void SupervisorTask_StartSupervision(void)
{
  // Notify supervised tasks
  MotorDrivingTask_Notify(MTR_EVT_ISALIVE);

  if( xTimerStart( gbl_sSupervisorTimerHandle, 0u /* no block time */ ) != pdPASS )
  {
    Error_Handler(true, ERR_OS_SUPERVISOR_TASK, ERR_TYPE_FAULT);
  }
}