#ifndef _ORDER_HANDLER_CONFIG_H_
#define _ORDER_HANDLER_CONFIG_H_

#include <stdint.h>
#include "uart_config.h"

#define ORDER_MAX_SIZE        (UART_MAX_SIZE)
#define ORDER_NB_MAX          (6u)
#define ORDER_HEADING_LENGTH  (2u)


typedef struct
{
  uint8_t u8Header;
  uint8_t u8Size;
  uint8_t au8Data[ORDER_MAX_SIZE - ORDER_HEADING_LENGTH];
  #if (ORDER_MAX_SIZE % 4u != 0)
    uint8_t au8Padding[(ORDER_MAX_SIZE % 4)]; // in case OrderHandler_Message_t is not aligned to 32bits
  #endif
}__attribute__((packed)) OrderHandler_Message_t;

typedef void (*fpOrderCallback)(OrderHandler_Message_t*);

typedef enum
{
  ORDER_MTR_START = '1',
  ORDER_MTR_STOP = '2',

  ORDER_GET_CURRENT = '3',
  ORDER_GET_SPEED = '4',

  ORDER_BAD_ACK = 0xFF,
  ORDER_GOOD_ACK = 0x00
} Orders_t;

typedef struct
{
  Orders_t eOrder;
  fpOrderCallback fpCallback;
} OrderDispatch_t;

#endif // _ORDER_HANDLER_CONFIG_H_