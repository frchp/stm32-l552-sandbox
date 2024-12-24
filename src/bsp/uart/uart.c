#include <stddef.h>
#include <string.h> // for memcpy

#include "stm32l5xx_ll_dma.h"
#include "stm32l5xx_ll_lpuart.h"
#include "stm32l5xx_ll_gpio.h"
#include "stm32l5xx_ll_bus.h"
#include "stm32l5xx_ll_rcc.h"
#include "stm32l5xx_ll_pwr.h"

#include "uart.h"
#include "uart_config.h"

#include "cpu_delay.h"

#define UART_DELAY_REG_US (200u)

static fpUartCallback gbl_fpTxListener = NULL;
static fpUartCallback gbl_fpRxListener = NULL;

static uint8_t gbl_au8RxBuffer[UART_MAX_SIZE];
static uint8_t gbl_u8RxPosition = 0u;

/**
  @brief Setup the Uart.
 */
void Uart_Init(void)
{
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

  NVIC_SetPriority(LPUART1_IRQn, 0);
  NVIC_EnableIRQ(LPUART1_IRQn);

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
  LL_LPUART_Enable(LPUART1);

  while ((!(LL_LPUART_IsActiveFlag_TEACK(LPUART1))) || (!(LL_LPUART_IsActiveFlag_REACK(LPUART1))))
  {
  }
}

/**
  @brief Activate Uart.
 */
void Uart_Activate(void)
{
  // Clear flags
  WRITE_REG(LPUART1->ICR, UINT32_MAX);
  cpu_delay_WaitFor(UART_DELAY_REG_US);

  /* Enable RXNE and Error interrupts */
  LL_LPUART_EnableIT_RXNE(LPUART1);
  LL_LPUART_EnableIT_ERROR(LPUART1);
}

/**
  @brief Transmit given buffer.
 */
void Uart_Transmit(uint8_t* arg_au8Buffer)
{
  /* Send characters one per one, until last char to be sent */
  for (uint8_t loc_u8index = 0; loc_u8index < UART_MAX_SIZE; loc_u8index++)
  {
    /* Wait for TXE flag to be raised */
    while (!LL_LPUART_IsActiveFlag_TXE(LPUART1))
    {
    }

    /* Write character in Transmit Data register.
       TXE flag is cleared by writing data in TDR register */
    LL_LPUART_TransmitData8(LPUART1, arg_au8Buffer[loc_u8index]);
  }

  /* Wait for TC flag to be raised for last char */
  while (!LL_LPUART_IsActiveFlag_TC(LPUART1))
  {
  }

  Uart_TxComplete();
}

/**
  @brief Receive given buffer.
 */
void Uart_Receive(uint8_t* arg_au8Buffer)
{
  memcpy(arg_au8Buffer, gbl_au8RxBuffer, UART_MAX_SIZE);
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
  gbl_au8RxBuffer[gbl_u8RxPosition] = LL_LPUART_ReceiveData8(LPUART1);
  if(gbl_au8RxBuffer[gbl_u8RxPosition] == UART_END_OF_MSG_CHAR)
  {
    if(gbl_fpRxListener != NULL)
    {
      gbl_fpRxListener();
    }
    gbl_u8RxPosition = 0u;
    memset(gbl_au8RxBuffer, 0u, UART_MAX_SIZE); // clear buffer
  }
  else
  {
    if(gbl_u8RxPosition < UART_MAX_SIZE)
    {
      gbl_u8RxPosition++;
    }
    else
    {
      gbl_u8RxPosition = 0u;
    }
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
