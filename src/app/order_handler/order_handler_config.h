#ifndef _ORDER_HANDLER_CONFIG_H_
#define _ORDER_HANDLER_CONFIG_H_

#include <stdint.h>
#include "uart_config.h"

typedef void (*fpOrderCallback)(uint8_t*);

#define ORDER_MAX_SIZE (UART_MAX_SIZE)
#define ORDER_NB_MAX (6u)
#define ORDER_IDX_CMD (0u)
#define ORDER_IDX_ARG_1 (1u)
#define ORDER_IDX_ARG_2 (2u)
#define ORDER_IDX_ARG_3 (3u)
#define ORDER_IDX_ARG_4 (4u)

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