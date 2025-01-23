#include <stddef.h>
#include <stdbool.h>
#include <string.h> // for memcpy

#include "order_handler.h"
#include "order_handler_config.h"

#include "uart.h"
#include "motor_controller.h"
#include "speed_sensor.h"
#include "intensity_sensor.h"
#include "media_task.h"
#include "motor_task.h"
#include "error.h"

typedef enum
{
  OH_STATE_WAIT,
  OH_STATE_HEADER_RECEIVED,
  OH_STATE_SIZE_RECEIVED,
  OH_STATE_DATA,
  OH_STATE_LAST = OH_STATE_DATA
} OrderHandler_State_t;

/* RX variables */
static OrderHandler_State_t gbl_eRxState = OH_STATE_WAIT;
static Motor_Control_t *gbl_psMtrControl; // To save motor order to send
static OrderHandler_Message_t gbl_sRxMessage;
static uint8_t gbl_u8RxToRead = 0u;

/* TX variables */
static OrderHandler_Message_t gbl_sTxMessage;

#define NB_BYTES_IN_32BITS (4u)
#define BYTE_MASK (0xFF)
#define GET_MSB1(arg_u32Val) ((arg_u32Val >> 24u) & BYTE_MASK)
#define GET_MSB2(arg_u32Val) ((arg_u32Val >> 16u) & BYTE_MASK)
#define GET_LSB2(arg_u32Val) ((arg_u32Val >> 8u) & BYTE_MASK)
#define GET_LSB1(arg_u32Val) (arg_u32Val & BYTE_MASK)

static void OrderHandler_ACTION_Decode(void);
static bool OrderHander_GUARD_IsHeaderValid(uint8_t arg_u8RxByte);
static void OrderHandler_PRV_Send(Orders_t arg_eCmd, uint8_t *arg_au8Data, uint8_t arg_u8DataNbBytes);
static OrderDispatch_t OrderHandler_PRV_FindCommand(Orders_t arg_eCmd);
static void OrderHandler_PRV_Start(OrderHandler_Message_t* arg_asMsg);
static void OrderHandler_PRV_Stop(OrderHandler_Message_t* arg_asMsg);
static void OrderHandler_PRV_GetCurrent(OrderHandler_Message_t* arg_asMsg);
static void OrderHandler_PRV_GetSpeed(OrderHandler_Message_t* arg_asMsg);

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
  switch(gbl_eRxState)
  {
    case OH_STATE_WAIT:
      Uart_Receive(&(gbl_sRxMessage.u8Header), 1u); // Read byte by byte
      if(OrderHander_GUARD_IsHeaderValid(gbl_sRxMessage.u8Header))
      {
        gbl_eRxState = OH_STATE_HEADER_RECEIVED;
      }
      else
      {
        // Unknown header, stay in waiting state
      }
      break;

    case OH_STATE_HEADER_RECEIVED:
      Uart_Receive(&(gbl_sRxMessage.u8Size), 1u); // Read byte by byte
      if(gbl_sRxMessage.u8Size <= ORDER_MAX_SIZE)
      {
        // Valid size
        if(gbl_sRxMessage.u8Size <= ORDER_HEADING_LENGTH)
        {
          // Frame has no data, end it here
          OrderHandler_ACTION_Decode();
          gbl_eRxState = OH_STATE_WAIT;
        }
        else
        {
          gbl_u8RxToRead = gbl_sRxMessage.u8Size - ORDER_HEADING_LENGTH;
          gbl_eRxState = OH_STATE_SIZE_RECEIVED;
        }
      }
      else
      {
        // Invalid size, go back to waiting
        gbl_eRxState = OH_STATE_WAIT;
      }
      break;

    case OH_STATE_SIZE_RECEIVED:
      Uart_Receive(&gbl_sRxMessage.au8Data[gbl_sRxMessage.u8Size - ORDER_HEADING_LENGTH - gbl_u8RxToRead], 1u); // Read byte by byte
      gbl_u8RxToRead--;

      if(gbl_u8RxToRead == 0u)
      {
        OrderHandler_ACTION_Decode();
        gbl_eRxState = OH_STATE_WAIT;
      }
      else
      {
        // More data to be read
      }
      break;

    default:
      Error_Handler();
      break;
  }
}

static void OrderHandler_ACTION_Decode(void)
{
  OrderDispatch_t loc_sOrder = OrderHandler_PRV_FindCommand(gbl_sRxMessage.u8Header);
  if(loc_sOrder.fpCallback != NULL)
  {
    loc_sOrder.fpCallback(&gbl_sRxMessage);
  }
  else
  {
    OrderHandler_PRV_Send(ORDER_BAD_ACK, NULL, 0);
  }
}

static bool OrderHander_GUARD_IsHeaderValid(uint8_t arg_u8RxByte)
{
  bool loc_bRet = false;

  switch(arg_u8RxByte)
  {
    case (uint8_t)ORDER_MTR_START:
    case (uint8_t)ORDER_MTR_STOP:
    case (uint8_t)ORDER_GET_CURRENT:
    case (uint8_t)ORDER_GET_SPEED:
    case (uint8_t)ORDER_BAD_ACK:
    case (uint8_t)ORDER_GOOD_ACK:
      // Command recognized
      loc_bRet = true;
      break;

    default:
      // Not recognized
      break;
  }

  return loc_bRet;
}

static void OrderHandler_PRV_Send(Orders_t arg_eCmd, uint8_t *arg_au8Data, uint8_t arg_u8DataNbBytes)
{
  // We use a global variable so that the pointer does not expire during transmission
  gbl_sTxMessage.u8Header = (uint8_t)arg_eCmd;
  gbl_sTxMessage.u8Size = ORDER_HEADING_LENGTH + arg_u8DataNbBytes;
  if((arg_u8DataNbBytes > 0) && (arg_au8Data != NULL))
  {
    memcpy(gbl_sTxMessage.au8Data, arg_au8Data, arg_u8DataNbBytes);
  }
  Uart_Transmit((uint8_t*)&gbl_sTxMessage, ORDER_HEADING_LENGTH + arg_u8DataNbBytes);
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

static void OrderHandler_PRV_Start(OrderHandler_Message_t* arg_asMsg)
{
  gbl_psMtrControl->u32Speed = (uint32_t)arg_asMsg->au8Data[0];
  gbl_psMtrControl->eDirection = (MotorDirection_t)arg_asMsg->au8Data[1];
  MotorDrivingTask_Run(gbl_psMtrControl);
}

static void OrderHandler_PRV_Stop(OrderHandler_Message_t* arg_asMsg)
{
  MotorDrivingTask_Notify(MTR_EVT_STOP);
}

static void OrderHandler_PRV_GetCurrent(OrderHandler_Message_t* arg_asMsg)
{
  uint32_t loc_u32CurrentInMilliAmp = IntensitySensor_Get();
  uint8_t loc_u8TxData[NB_BYTES_IN_32BITS] =
  {
    GET_MSB1(loc_u32CurrentInMilliAmp),
    GET_MSB2(loc_u32CurrentInMilliAmp),
    GET_LSB2(loc_u32CurrentInMilliAmp),
    GET_LSB1(loc_u32CurrentInMilliAmp)
  };
  OrderHandler_PRV_Send(ORDER_GOOD_ACK, loc_u8TxData, NB_BYTES_IN_32BITS);
}

static void OrderHandler_PRV_GetSpeed(OrderHandler_Message_t* arg_asMsg)
{
  uint32_t loc_u32Speed = SpeedSensor_Get();
  uint8_t loc_u8TxData[NB_BYTES_IN_32BITS] =
  {
    GET_MSB1(loc_u32Speed),
    GET_MSB2(loc_u32Speed),
    GET_LSB2(loc_u32Speed),
    GET_LSB1(loc_u32Speed)
  };
  OrderHandler_PRV_Send(ORDER_GOOD_ACK, loc_u8TxData, NB_BYTES_IN_32BITS);
}

