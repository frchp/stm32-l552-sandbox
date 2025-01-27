#include <stddef.h>
#include <string.h> // for memcpy
#include <stdbool.h>

#include "stm32l5xx_ll_dma.h"
#include "stm32l5xx_ll_lpuart.h"
#include "stm32l5xx_ll_gpio.h"
#include "stm32l5xx_ll_bus.h"
#include "stm32l5xx_ll_rcc.h"
#include "stm32l5xx_ll_pwr.h"

#include "uart.h"
#include "uart_config.h"

#include "cpu_delay.h"
#include "interrupts.h"
#include "error.h"
#include "ring_buffer.h"

#define UART_DELAY_REG_US (200u)

static fpUartCallback gbl_fpTxListener = NULL;
static fpUartCallback gbl_fpRxListener = NULL;

RingBuffer_t gbl_sRxBuffer;

static uint8_t *gbl_au8TxBuffer;
static uint8_t gbl_u8TxSize = 0u;

/**
  @brief Setup the Uart.
 */
void Uart_Init(void)
{
  static bool loc_bInitialized = false;
  if(!loc_bInitialized)
  {
    loc_bInitialized = true;

    // Initialize Rx buffer
    if(!RingBuffer_Init(&gbl_sRxBuffer, 2*UART_MAX_SIZE))
    {
      Error_Handler(true, ERR_BSP_UART, ERR_TYPE_INIT);
    }

    LL_LPUART_InitTypeDef LPUART_InitStruct = {0};
    LL_GPIO_InitTypeDef GPIO_InitStruct = {0};

    LL_RCC_SetLPUARTClockSource(LL_RCC_LPUART1_CLKSOURCE_PCLK1);

    /* Peripheral clock enable */
    LL_APB1_GRP2_EnableClock(LL_APB1_GRP2_PERIPH_LPUART1);

    LL_AHB2_GRP1_EnableClock(LL_AHB2_GRP1_PERIPH_GPIOG);
    LL_PWR_EnableVddIO2();

    /**LPUART1 GPIO Configuration
    PG7   ------> LPUART1_TX
    PG8   ------> LPUART1_RX
    */
    GPIO_InitStruct.Pin = LL_GPIO_PIN_7|LL_GPIO_PIN_8;
    GPIO_InitStruct.Mode = LL_GPIO_MODE_ALTERNATE;
    GPIO_InitStruct.Speed = LL_GPIO_SPEED_FREQ_HIGH;
    GPIO_InitStruct.OutputType = LL_GPIO_OUTPUT_PUSHPULL;
    GPIO_InitStruct.Pull = LL_GPIO_PULL_UP;
    GPIO_InitStruct.Alternate = LL_GPIO_AF_8;
    LL_GPIO_Init(GPIOG, &GPIO_InitStruct);

    Interrupts_Enable(INT_UART);

    LPUART_InitStruct.PrescalerValue = LL_LPUART_PRESCALER_DIV1;
    LPUART_InitStruct.BaudRate = 115200;
    LPUART_InitStruct.DataWidth = LL_LPUART_DATAWIDTH_8B;
    LPUART_InitStruct.StopBits = LL_LPUART_STOPBITS_1;
    LPUART_InitStruct.Parity = LL_LPUART_PARITY_NONE;
    LPUART_InitStruct.TransferDirection = LL_LPUART_DIRECTION_TX_RX;
    LPUART_InitStruct.HardwareFlowControl = LL_LPUART_HWCONTROL_NONE;
    LL_LPUART_Init(LPUART1, &LPUART_InitStruct);
    LL_LPUART_SetTXFIFOThreshold(LPUART1, LL_LPUART_FIFOTHRESHOLD_1_8);
    LL_LPUART_SetRXFIFOThreshold(LPUART1, LL_LPUART_FIFOTHRESHOLD_1_8);
    LL_LPUART_DisableFIFO(LPUART1);
  }
}

/**
  @brief Activate Uart.
 */
void Uart_Activate(void)
{
  LL_LPUART_Enable(LPUART1);
  while ((!(LL_LPUART_IsActiveFlag_TEACK(LPUART1))) || (!(LL_LPUART_IsActiveFlag_REACK(LPUART1))));

  // Clear flags
  LL_LPUART_ClearFlag_TC(LPUART1);

  /* Enable RXNE, TC interrupts */
  LL_LPUART_EnableIT_RXNE(LPUART1);
  LL_LPUART_EnableIT_TC(LPUART1);
  LL_LPUART_EnableIT_ERROR(LPUART1);
}

/**
  @brief Transmit given buffer.
 */
void Uart_Transmit(uint8_t* arg_au8Buffer, uint8_t arg_u8Size)
{
  gbl_au8TxBuffer = arg_au8Buffer;
  gbl_u8TxSize = arg_u8Size;

  // Send first byte
  while (!LL_LPUART_IsActiveFlag_TXE(LPUART1));
  /* Write character in Transmit Data register.
       TXE flag is cleared by writing data in TDR register */
  LL_LPUART_TransmitData8(LPUART1, *gbl_au8TxBuffer);
  gbl_u8TxSize--;
  gbl_au8TxBuffer++;

  if(gbl_u8TxSize == 0u)
  {
    // Frame was 1 byte long
    Uart_TxComplete();
  }
  else
  {
    // Rest of frame will be sent from interrupt
  }
}

/**
  @brief Receive given buffer.
 */
void Uart_Receive(uint8_t* arg_au8Buffer, uint8_t arg_u8Size)
{
  for(uint8_t loc_u8Idx = 0u; loc_u8Idx < arg_u8Size; loc_u8Idx++)
  {
    arg_au8Buffer[loc_u8Idx] = RingBuffer_Remove(&gbl_sRxBuffer);
  }
}

/**
  @brief Callback.
 */
void Uart_TxComplete(void)
{
  if(gbl_fpTxListener != NULL)
  {
    gbl_fpTxListener();
  }
}

/**
  @brief Callback.
 */
void Uart_RxByteComplete(void)
{
  if(RingBuffer_Add(&gbl_sRxBuffer, LL_LPUART_ReceiveData8(LPUART1)))
  {
    if(gbl_fpRxListener != NULL)
    {
      gbl_fpRxListener();
    }
  }
  else
  {
    Error_Handler(false, ERR_BSP_UART, ERR_TYPE_SAVE);
  }
}

/**
  @brief Callback.
 */
void Uart_TxByteComplete(void)
{
  /* Write character in Transmit Data register.
       TXE flag is cleared by writing data in TDR register */
  LL_LPUART_TransmitData8(LPUART1, *gbl_au8TxBuffer);
  gbl_u8TxSize--;
  gbl_au8TxBuffer++;

  if(gbl_u8TxSize == 0u)
  {
    Uart_TxComplete();
  }
  else
  {
    // Rest of frame will be sent from interrupt
  }
}

/**
  @brief Attach listeners.
 */
void Uart_Attach(fpUartCallback arg_fpTxListener, fpUartCallback arg_fpRxListener)
{
  if(arg_fpTxListener != NULL)
  {
    gbl_fpTxListener = arg_fpTxListener;
  }
  if(arg_fpRxListener != NULL)
  {
    gbl_fpRxListener = arg_fpRxListener;
  }
}
