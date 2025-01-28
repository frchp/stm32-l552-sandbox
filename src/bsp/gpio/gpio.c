#include <stdbool.h>
#include <stdint.h>

#include "gpio.h"

#include "board.h"
#include "clocks.h"
#include "reg_macro.h"
#include "stm32l552xx.h"

#define GPIO_MODE_INPUT                 (0x00000000U)
#define GPIO_MODE_OUTPUT                GPIO_MODER_MODE0_0
#define GPIO_MODE_ALTERNATE             GPIO_MODER_MODE0_1
#define GPIO_MODE_ANALOG                GPIO_MODER_MODE0

#define GPIO_PULL_NO                    (0x00000000U)
#define GPIO_PULL_UP                    GPIO_PUPDR_PUPD0_0
#define GPIO_PULL_DOWN                  GPIO_PUPDR_PUPD0_1

#define GPIO_SPEED_LOW                  (0x00000000U)
#define GPIO_SPEED_MEDIUM               GPIO_OSPEEDR_OSPEED0_0
#define GPIO_SPEED_FAST                 GPIO_OSPEEDR_OSPEED0_1
#define GPIO_SPEED_HIGH                 GPIO_OSPEEDR_OSPEED0

#define GPIO_AF_0                       (0x0000000U)
#define GPIO_AF_1                       (0x0000001U)
#define GPIO_AF_2                       (0x0000002U)
#define GPIO_AF_3                       (0x0000003U)
#define GPIO_AF_4                       (0x0000004U)
#define GPIO_AF_5                       (0x0000005U)
#define GPIO_AF_6                       (0x0000006U)
#define GPIO_AF_7                       (0x0000007U)
#define GPIO_AF_8                       (0x0000008U)
#define GPIO_AF_9                       (0x0000009U)
#define GPIO_AF_10                      (0x000000AU)
#define GPIO_AF_11                      (0x000000BU)
#define GPIO_AF_12                      (0x000000CU)
#define GPIO_AF_13                      (0x000000DU)
#define GPIO_AF_14                      (0x000000EU)
#define GPIO_AF_15                      (0x000000FU)

#define GPIO_OUTPUT_PUSHPULL            (0x00000000U)
#define GPIO_OUTPUT_OPENDRAIN           GPIO_OTYPER_OT0

static void Gpio_SetPinMode(GPIO_TypeDef *arg_GpioReg, uint32_t arg_u32Pin, uint32_t arg_u32Mode);
static void Gpio_SetPinPull(GPIO_TypeDef *arg_GpioReg, uint32_t arg_u32Pin, uint32_t arg_u32Pull);
static void Gpio_SetPinSpeed(GPIO_TypeDef *arg_GpioReg, uint32_t arg_u32Pin, uint32_t arg_u32Speed);
static void Gpio_SetAlternateFunction(GPIO_TypeDef *arg_GpioReg, uint32_t arg_u32Pin, uint32_t arg_u32AlternateFunction);
static void Gpio_SetPinOutputType(GPIO_TypeDef *arg_GpioReg, uint32_t arg_u32Pin, uint32_t arg_u32Type);

/**
  @brief Setup the GPIO defined in configuration.
 */
void Gpio_Init(void)
{
  static bool loc_bInitialized = false;
  if(!loc_bInitialized)
  {
    loc_bInitialized = true;

    /** ADC1 GPIO Configuration for BOARD_ADC_MTR_CURRENT */
    Clocks_Enable_AHB2_GRP1(RCC_AHB2ENR_GPIOCEN);
    Gpio_SetPinMode(BOARD_ADC_MTR_CURRENT_PORT, BOARD_ADC_MTR_CURRENT_PIN, GPIO_MODE_ANALOG);
    Gpio_SetPinPull(BOARD_ADC_MTR_CURRENT_PORT, BOARD_ADC_MTR_CURRENT_PIN, GPIO_PULL_NO);

    /** TIM1_CH1 GPIO Configuration for BOARD_GPIO_MTR_FB */
    Clocks_Enable_AHB2_GRP1(RCC_AHB2ENR_GPIOAEN);
    Gpio_SetPinMode(BOARD_GPIO_MTR_FB_PORT, BOARD_GPIO_MTR_FB_PIN, GPIO_MODE_ALTERNATE);
    Gpio_SetPinPull(BOARD_GPIO_MTR_FB_PORT, BOARD_GPIO_MTR_FB_PIN, GPIO_PULL_UP);
    Gpio_SetPinSpeed(BOARD_GPIO_MTR_FB_PORT, BOARD_GPIO_MTR_FB_PIN, GPIO_SPEED_HIGH);
    Gpio_SetAlternateFunction(BOARD_GPIO_MTR_FB_PORT, BOARD_GPIO_MTR_FB_PIN, GPIO_AF_1);

    /** TIM3_CH2 GPIO Configuration for BOARD_GPIO_MTR_CTRL */
    Gpio_SetPinMode(BOARD_GPIO_MTR_CTRL_PORT, BOARD_GPIO_MTR_CTRL_PIN, GPIO_MODE_ALTERNATE);
    Gpio_SetPinPull(BOARD_GPIO_MTR_CTRL_PORT, BOARD_GPIO_MTR_CTRL_PIN, GPIO_PULL_DOWN);
    Gpio_SetPinSpeed(BOARD_GPIO_MTR_CTRL_PORT, BOARD_GPIO_MTR_CTRL_PIN, GPIO_SPEED_HIGH);
    Gpio_SetAlternateFunction(BOARD_GPIO_MTR_CTRL_PORT, BOARD_GPIO_MTR_CTRL_PIN, GPIO_AF_2);

    /* Direction GPIO */
    Gpio_SetPinMode(BOARD_GPIO_MTR_DIR_PORT, BOARD_GPIO_MTR_DIR_PIN, GPIO_MODE_OUTPUT);
    Gpio_SetPinOutputType(BOARD_GPIO_MTR_DIR_PORT, BOARD_GPIO_MTR_DIR_PIN, GPIO_OUTPUT_PUSHPULL);
    Gpio_SetPinSpeed(BOARD_GPIO_MTR_DIR_PORT, BOARD_GPIO_MTR_DIR_PIN, GPIO_SPEED_LOW);

    /* Alive LED GPIO */
    Gpio_SetPinMode(BOARD_GPIO_LED_ALIVE_PORT, BOARD_GPIO_LED_ALIVE_PIN, GPIO_MODE_OUTPUT);
    Gpio_SetPinOutputType(BOARD_GPIO_LED_ALIVE_PORT, BOARD_GPIO_LED_ALIVE_PIN, GPIO_OUTPUT_PUSHPULL);
    Gpio_SetPinSpeed(BOARD_GPIO_LED_ALIVE_PORT, BOARD_GPIO_LED_ALIVE_PIN, GPIO_SPEED_LOW);

    /* UART RX GPIO */
    Clocks_Enable_AHB2_GRP1(RCC_AHB2ENR_GPIOGEN);
    Gpio_SetPinMode(BOARD_GPIO_UART_RX_PORT, BOARD_GPIO_UART_RX_PIN, GPIO_MODE_ALTERNATE);
    Gpio_SetPinSpeed(BOARD_GPIO_UART_RX_PORT, BOARD_GPIO_UART_RX_PIN, LL_GPIO_SPEED_FREQ_HIGH);
    Gpio_SetPinPull(BOARD_GPIO_UART_RX_PORT, BOARD_GPIO_UART_RX_PIN, GPIO_PULL_UP);
    Gpio_SetAlternateFunction(BOARD_GPIO_UART_RX_PORT, BOARD_GPIO_UART_RX_PIN, GPIO_AF_8);

    /* UART TX GPIO */
    Gpio_SetPinMode(BOARD_GPIO_LED_ALIVE_PORT, BOARD_GPIO_LED_ALIVE_PIN, GPIO_MODE_ALTERNATE);
    Gpio_SetPinSpeed(BOARD_GPIO_LED_ALIVE_PORT, BOARD_GPIO_LED_ALIVE_PIN, LL_GPIO_SPEED_FREQ_HIGH);
    Gpio_SetPinOutputType(BOARD_GPIO_LED_ALIVE_PORT, BOARD_GPIO_LED_ALIVE_PIN, GPIO_OUTPUT_PUSHPULL);
    Gpio_SetAlternateFunction(BOARD_GPIO_MTR_FB_PORT, BOARD_GPIO_MTR_FB_PIN, GPIO_AF_8);
  }
}

/**
  @brief Set given GPIO to given state (true = HIGH, false = LOW).
 */
void Gpio_Set(GPIO_TypeDef *arg_GpioReg, uint32_t arg_u32Pin, bool arg_bLevel)
{
  if(arg_bLevel)
  {
    WRITE_REG(arg_GpioReg->BSRR, arg_u32Pin);
  }
  else
  {
    WRITE_REG(arg_GpioReg->BRR, arg_u32Pin);
  }
}

/**
  @brief Toggle given GPIO.
 */
void Gpio_Toggle(GPIO_TypeDef *arg_GpioReg, uint32_t arg_u32Pin)
{
  uint32_t loc_u32odr = READ_REG(arg_GpioReg->ODR); // Read output register
  WRITE_REG(arg_GpioReg->BSRR, ((loc_u32odr & arg_u32Pin) << 16u) | (~loc_u32odr & arg_u32Pin));
}

static void Gpio_SetPinMode(GPIO_TypeDef *arg_GpioReg, uint32_t arg_u32Pin, uint32_t arg_u32Mode)
{
  MODIFY_REG(arg_GpioReg->MODER, (GPIO_MODER_MODE0 << (POSITION_VAL(arg_u32Pin) * 2U)), (arg_u32Mode << (POSITION_VAL(arg_u32Pin) * 2U)));
}

static void Gpio_SetPinPull(GPIO_TypeDef *arg_GpioReg, uint32_t arg_u32Pin, uint32_t arg_u32Pull)
{
  MODIFY_REG(arg_GpioReg->PUPDR, (GPIO_PUPDR_PUPD0 << (POSITION_VAL(arg_u32Pin) * 2U)), (arg_u32Pull << (POSITION_VAL(arg_u32Pin) * 2U)));
}

static void Gpio_SetPinSpeed(GPIO_TypeDef *arg_GpioReg, uint32_t arg_u32Pin, uint32_t arg_u32Speed)
{
  MODIFY_REG(arg_GpioReg->OSPEEDR, (GPIO_OSPEEDR_OSPEED0 << (POSITION_VAL(arg_u32Pin) * 2U)),
             (arg_u32Speed << (POSITION_VAL(arg_u32Pin) * 2U)));
}

static void Gpio_SetAlternateFunction(GPIO_TypeDef *arg_GpioReg, uint32_t arg_u32Pin, uint32_t arg_u32AlternateFunction)
{
  if(arg_u32Pin < GPIO_BSRR_BS8)
  {
    // For 0 <= Pin < 7
    MODIFY_REG(arg_GpioReg->AFR[0], (GPIO_AFRL_AFSEL0 << (POSITION_VAL(arg_u32Pin) * 4U)),
              (arg_u32AlternateFunction << (POSITION_VAL(arg_u32Pin) * 4U)));
  }
  else
  {
    // For 7 < Pin <= 15
    MODIFY_REG(arg_GpioReg->AFR[1], (GPIO_AFRH_AFSEL8 << (POSITION_VAL(arg_u32Pin >> 8U) * 4U)),
              (arg_u32AlternateFunction << (POSITION_VAL(arg_u32Pin >> 8U) * 4U)));
  }
}

static void Gpio_SetPinOutputType(GPIO_TypeDef *arg_GpioReg, uint32_t arg_u32Pin, uint32_t arg_u32Type)
{
  MODIFY_REG(arg_GpioReg->OTYPER, arg_u32Pin, (arg_u32Pin * arg_u32Type));
}
