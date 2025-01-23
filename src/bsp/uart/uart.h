#ifndef _UART_H_
#define _UART_H_

#include <stdint.h>

typedef void (*fpUartCallback)(void);

/**
  @brief Setup the Uart.
 */
void Uart_Init(void);

/**
  @brief Activate Uart.
 */
void Uart_Activate(void);

/**
  @brief Transmit given buffer.
 */
void Uart_Transmit(uint8_t* arg_au8Buffer, uint8_t arg_u8Size);

/**
  @brief Receive given buffer.
 */
void Uart_Receive(uint8_t* arg_au8Buffer, uint8_t arg_u8Size);

/**
  @brief Callback.
 */
void Uart_TxComplete(void);

/**
  @brief Callback.
 */
void Uart_RxByteComplete(void);

/**
  @brief Callback.
 */
void Uart_TxByteComplete(void);

/**
  @brief Attach listeners.
 */
void Uart_Attach(fpUartCallback arg_fpTxListener, fpUartCallback arg_fpRxListener);

#endif // _UART_H_