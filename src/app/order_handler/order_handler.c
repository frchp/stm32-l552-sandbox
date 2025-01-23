#include <stddef.h>
#include "order_handler.h"
#include "order_handler_config.h"

#include "uart.h"
#include "motor_controller.h"
#include "speed_sensor.h"
#include "intensity_sensor.h"
#include "media_task.h"
#include "motor_task.h"

static Motor_Control_t *gbl_psMtrControl;

#define BYTE_MASK (0xFF)
#define GET_MSB1(arg_u32Val) ((arg_u32Val >> 24u) & BYTE_MASK)
#define GET_MSB2(arg_u32Val) ((arg_u32Val >> 16u) & BYTE_MASK)
#define GET_LSB2(arg_u32Val) ((arg_u32Val >> 8u) & BYTE_MASK)
#define GET_LSB1(arg_u32Val) (arg_u32Val & BYTE_MASK)

static void OrderHandler_PRV_Send(Orders_t arg_eCmd, uint32_t arg_u32FacultativeData);
static OrderDispatch_t OrderHandler_PRV_FindCommand(Orders_t arg_eCmd);
static void OrderHandler_PRV_Start(uint8_t* arg_au8Msg);
static void OrderHandler_PRV_Stop(uint8_t* arg_au8Msg);
static void OrderHandler_PRV_GetCurrent(uint8_t* arg_au8Msg);
static void OrderHandler_PRV_GetSpeed(uint8_t* arg_au8Msg);

static const OrderDispatch_t gbl_csDispatcher[ORDER_NB_MAX] =
{
  {ORDER_MTR_START, OrderHandler_PRV_Start},
  {ORDER_MTR_STOP, OrderHandler_PRV_Stop},
  {ORDER_GET_CURRENT, OrderHandler_PRV_GetCurrent},
  {ORDER_GET_SPEED, OrderHandler_PRV_GetSpeed},
  {ORDER_BAD_ACK, NULL},
  {ORDER_GOOD_ACK, NULL}
};

/**
  @brief Setup OrderHandler.
 */
void OrderHandler_Init(void)
{
  Uart_Attach(OrderHandler_MessageTransmitted, OrderHandler_MessageReceived);
}

/**
  @brief OrderHandler is notified message has been received
 */
void OrderHandler_MessageReceived(void)
{
  // uint8_t loc_au8IncomingMsg[ORDER_MAX_SIZE]; // TODO : use a ring buffer here to copy it inside
  // Uart_Receive(loc_au8IncomingMsg);
  MediaTask_OnReception();
}

/**
  @brief OrderHandler is notified message has been transmitted
 */
void OrderHandler_MessageTransmitted(void)
{
  // Nothing to do
}

/**
  @brief OrderHandler needs to decode a message.
 */
void OrderHandler_HandleRx(void)
{
  uint8_t loc_au8IncomingMsg[ORDER_MAX_SIZE]; // TODO : use a ring buffer to find command
  Uart_Receive(loc_au8IncomingMsg);
  OrderDispatch_t loc_sOrder = OrderHandler_PRV_FindCommand(loc_au8IncomingMsg[ORDER_IDX_CMD]);
  if(loc_sOrder.fpCallback != NULL)
  {
    loc_sOrder.fpCallback(loc_au8IncomingMsg);
  }
  else
  {
    OrderHandler_PRV_Send(ORDER_BAD_ACK, 0);
  }
}

static void OrderHandler_PRV_Send(Orders_t arg_eCmd, uint32_t arg_u32FacultativeData)
{
  uint8_t loc_au8TransmitMsg[ORDER_MAX_SIZE];
  loc_au8TransmitMsg[ORDER_IDX_CMD] = arg_eCmd;
  loc_au8TransmitMsg[ORDER_IDX_ARG_1] = GET_MSB1(arg_u32FacultativeData);
  loc_au8TransmitMsg[ORDER_IDX_ARG_2] = GET_MSB2(arg_u32FacultativeData);
  loc_au8TransmitMsg[ORDER_IDX_ARG_3] = GET_LSB2(arg_u32FacultativeData);
  loc_au8TransmitMsg[ORDER_IDX_ARG_4] = GET_LSB1(arg_u32FacultativeData);
  Uart_Transmit(loc_au8TransmitMsg);
}

static OrderDispatch_t OrderHandler_PRV_FindCommand(Orders_t arg_eCmd)
{
  OrderDispatch_t loc_sOrderFound = {ORDER_BAD_ACK, NULL};
  for(uint8_t loc_u8Idx = 0u; loc_u8Idx < ORDER_NB_MAX; loc_u8Idx++)
  {
    if(arg_eCmd == gbl_csDispatcher[loc_u8Idx].eOrder)
    {
      loc_sOrderFound = gbl_csDispatcher[loc_u8Idx];
      break;
    }
  }
  return loc_sOrderFound;
}

static void OrderHandler_PRV_Start(uint8_t* arg_au8Msg)
{
  gbl_psMtrControl->u32Speed = (uint32_t)arg_au8Msg[ORDER_IDX_ARG_1];
  gbl_psMtrControl->eDirection = (MotorDirection_t)arg_au8Msg[ORDER_IDX_ARG_2];
  MotorDrivingTask_Run(gbl_psMtrControl);
}

static void OrderHandler_PRV_Stop(uint8_t* arg_au8Msg)
{
  MotorDrivingTask_Notify(MTR_EVT_STOP);
}

static void OrderHandler_PRV_GetCurrent(uint8_t* arg_au8Msg)
{
  uint32_t loc_u32CurrentInMilliAmp = IntensitySensor_Get();
  OrderHandler_PRV_Send(ORDER_GOOD_ACK, loc_u32CurrentInMilliAmp);
}

static void OrderHandler_PRV_GetSpeed(uint8_t* arg_au8Msg)
{
  uint32_t loc_u32Speed = SpeedSensor_Get();
  OrderHandler_PRV_Send(ORDER_GOOD_ACK, loc_u32Speed);
}

