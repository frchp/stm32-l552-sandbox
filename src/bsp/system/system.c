#include <stddef.h>
#include <stdbool.h>

#include "system.h"
#include "adc.h"
#include "adc_config.h"
#include "board.h"
#include "timer_counter.h"
#include "timer_pwm.h"
#include "uart.h"
#include "watchdog.h"

#include "core_cm33.h"
#include "system_stm32l5xx.h"
#include "stm32l5xx_ll_rcc.h"
#include "stm32l5xx_ll_icache.h"
#include "stm32l5xx_ll_bus.h"
#include "stm32l5xx_ll_pwr.h"
#include "stm32l5xx_ll_utils.h"
#include "stm32l5xx_ll_system.h"
#include "stm32l5xx_ll_gpio.h"
#include "stm32l5xx_ll_adc.h"

#define SYSTEM_FREQ (110000000)

// The SystemCoreClock variable is updated by calling CMSIS function SystemCoreClockUpdate()
uint32_t SystemCoreClock = 110000000;

const uint8_t  AHBPrescTable[16] = {0U, 0U, 0U, 0U, 0U, 0U, 0U, 0U, 1U, 2U, 3U, 4U, 6U, 7U, 8U, 9U};
const uint8_t  APBPrescTable[8] =  {0U, 0U, 0U, 0U, 1U, 2U, 3U, 4U};
const uint32_t MSIRangeTable[16] = {100000U,   200000U,   400000U,   800000U,  1000000U,  2000000U, \
                                    4000000U, 8000000U, 16000000U, 24000000U, 32000000U, 48000000U, \
                                    0U,       0U,       0U,        0U};

const SignalConfig_t gbl_sAdcSignalConfig[ADC_NB_SIGNALS] =
{
  {
    BOARD_ADC_MTR_CURRENT_CHANNEL,
    NULL
  }
};

#define NVIC_PRIORITYGROUP_0         ((uint32_t)0x00000007) /*!< 0 bit  for pre-emption priority,
                                                                 3 bits for subpriority */
#define NVIC_PRIORITYGROUP_1         ((uint32_t)0x00000006) /*!< 1 bit  for pre-emption priority,
                                                                 2 bits for subpriority */
#define NVIC_PRIORITYGROUP_2         ((uint32_t)0x00000005) /*!< 2 bits for pre-emption priority,
                                                                 1 bits for subpriority */
#define NVIC_PRIORITYGROUP_3         ((uint32_t)0x00000004) /*!< 3 bits for pre-emption priority,
                                                                 0 bit  for subpriority */

static void system_PRV_InitGpio(void);

/**
  @brief Setup the BSP.
 */
void SystemInit (void)
{
#if defined(USER_VECT_TAB_ADDRESS)
  SCB->VTOR = VECT_TAB_BASE_ADDRESS | VECT_TAB_OFFSET;
#endif

#if (__FPU_PRESENT == 1) && (__FPU_USED == 1)
  SCB->CPACR |= ((3UL << 20U)|(3UL << 22U));  /* set CP10 and CP11 Full Access */
#endif
}

/**
  @brief Update clocks based on register value.
 */
void SystemCoreClockUpdate(void)
{
  uint32_t tmp, msirange, pllvco, pllsource, pllm, pllr;

  if((RCC->CR & RCC_CR_MSIRGSEL) == 0U)
  { /* MSISRANGE from RCC_CSR applies */
    msirange = (RCC->CSR & RCC_CSR_MSISRANGE) >> 8U;
  }
  else
  { /* MSIRANGE from RCC_CR applies */
    msirange = (RCC->CR & RCC_CR_MSIRANGE) >> 4U;
  }
  /*MSI frequency range in HZ*/
  msirange = MSIRangeTable[msirange];

  switch (RCC->CFGR & RCC_CFGR_SWS)
  {
    case 0x00:  /* MSI used as system clock source */
      SystemCoreClock = msirange;
      break;

    case 0x04:  /* HSI used as system clock source */
      SystemCoreClock = HSI_VALUE;
      break;

    case 0x08:  /* HSE used as system clock source */
      SystemCoreClock = HSE_VALUE;
      break;

    case 0x0C:  /* PLL used as system clock  source */
      /* PLL_VCO = (HSE_VALUE or HSI_VALUE or MSI_VALUE/ PLLM) * PLLN
         SYSCLK = PLL_VCO / PLLR
         */
      pllsource = (RCC->PLLCFGR & RCC_PLLCFGR_PLLSRC);
      pllm = ((RCC->PLLCFGR & RCC_PLLCFGR_PLLM) >> 4U) + 1U ;

      switch (pllsource)
      {
        case 0x02:  /* HSI used as PLL clock source */
          pllvco = (HSI_VALUE / pllm);
          break;

        case 0x03:  /* HSE used as PLL clock source */
          pllvco = (HSE_VALUE / pllm);
          break;

        default:    /* MSI used as PLL clock source */
          pllvco = (msirange / pllm);
          break;
      }
      pllvco = pllvco * ((RCC->PLLCFGR & RCC_PLLCFGR_PLLN) >> 8U);
      pllr = (((RCC->PLLCFGR & RCC_PLLCFGR_PLLR) >> 25U) + 1U) * 2U;
      SystemCoreClock = pllvco/pllr;
      break;

    default:
      SystemCoreClock = msirange;
      break;
  }
  /* Get HCLK prescaler */
  tmp = AHBPrescTable[((RCC->CFGR & RCC_CFGR_HPRE) >> 4U)];
  /* HCLK clock frequency */
  SystemCoreClock >>= tmp;
}

/**
  @brief Setup the BSP.
 */
void Bsp_Init (void)
{
  Bsp_InitClock();

  system_PRV_InitGpio();

  Adc_Init(gbl_sAdcSignalConfig);
  TimerCounter_Init();
  TimerPwm_Init();
  Uart_Init();
}

/**
  @brief Setup the BSP.
 */
void Bsp_Activate (void)
{
  Adc_Activate();
  TimerCounter_Activate();
  TimerPwm_Activate();
  Uart_Activate();
  Watchdog_Activate(); // Last to be activated, to not trigger reset during Init
}

/**
  @brief Setup the clock tree.
 */
void Bsp_InitClock(void)
{
  LL_APB2_GRP1_EnableClock(LL_APB2_GRP1_PERIPH_SYSCFG);
  LL_APB1_GRP1_EnableClock(LL_APB1_GRP1_PERIPH_PWR);

  NVIC_SetPriorityGrouping(NVIC_PRIORITYGROUP_3);

  LL_PWR_DisableUCPDDeadBattery();

  LL_PWR_SetRegulVoltageScaling(LL_PWR_REGU_VOLTAGE_SCALE0);

  LL_FLASH_SetLatency(LL_FLASH_LATENCY_5);
  while(LL_FLASH_GetLatency()!= LL_FLASH_LATENCY_5);
  LL_PWR_EnableBkUpAccess();

  // Enable LSI
  LL_RCC_LSI_Enable();
  while(LL_RCC_LSI_IsReady() != 1);

  LL_RCC_MSI_Enable();
  while (LL_RCC_MSI_IsReady() != 1u);
  LL_RCC_MSI_EnableRangeSelection();
  LL_RCC_MSI_SetRange(LL_RCC_MSIRANGE_6);
  LL_RCC_MSI_SetCalibTrimming(0);

  /* Main PLL configuration and activation */
  LL_RCC_PLL_ConfigDomain_SYS(LL_RCC_PLLSOURCE_MSI, LL_RCC_PLLM_DIV_1, 55, LL_RCC_PLLR_DIV_2);
  LL_RCC_PLL_Enable();
  LL_RCC_PLL_EnableDomain_SYS();
  while(LL_RCC_PLL_IsReady() != 1);

  LL_RCC_SetSysClkSource(LL_RCC_SYS_CLKSOURCE_PLL);
  while(LL_RCC_GetSysClkSource() != LL_RCC_SYS_CLKSOURCE_STATUS_PLL);
  LL_RCC_SetAHBPrescaler(LL_RCC_SYSCLK_DIV_1);
  LL_RCC_SetAPB1Prescaler(LL_RCC_APB1_DIV_1);
  LL_RCC_SetAPB2Prescaler(LL_RCC_APB2_DIV_1);

  LL_Init1msTick(SYSTEM_FREQ);

  /* Update CMSIS variable (which can be updated also through SystemCoreClockUpdate function) */
  LL_SetSystemCoreClock(SYSTEM_FREQ);

  LL_ICACHE_SetMode(LL_ICACHE_1WAY);
  LL_ICACHE_Enable();
}

static void system_PRV_InitGpio(void)
{
  static bool loc_bInitialized = false;
  if(!loc_bInitialized)
  {
    loc_bInitialized = true;

    /** ADC1 GPIO Configuration for BOARD_ADC_MTR_CURRENT */
    LL_AHB2_GRP1_EnableClock(LL_AHB2_GRP1_PERIPH_GPIOC);
    LL_GPIO_SetPinMode(BOARD_ADC_MTR_CURRENT_PORT, BOARD_ADC_MTR_CURRENT_PIN, LL_GPIO_MODE_ANALOG);
    LL_GPIO_SetPinPull(BOARD_ADC_MTR_CURRENT_PORT, BOARD_ADC_MTR_CURRENT_PIN, LL_GPIO_PULL_NO);

    /** TIM1_CH1 GPIO Configuration for BOARD_GPIO_MTR_FB */
    LL_AHB2_GRP1_EnableClock(LL_AHB2_GRP1_PERIPH_GPIOA);
    LL_GPIO_SetPinMode(BOARD_GPIO_MTR_FB_PORT, BOARD_GPIO_MTR_FB_PIN, LL_GPIO_MODE_ALTERNATE);
    LL_GPIO_SetPinPull(BOARD_GPIO_MTR_FB_PORT, BOARD_GPIO_MTR_FB_PIN, LL_GPIO_PULL_UP);
    LL_GPIO_SetPinSpeed(BOARD_GPIO_MTR_FB_PORT, BOARD_GPIO_MTR_FB_PIN, LL_GPIO_SPEED_FREQ_HIGH);
    LL_GPIO_SetAFPin_0_7(BOARD_GPIO_MTR_FB_PORT, BOARD_GPIO_MTR_FB_PIN, LL_GPIO_AF_1);

    /** TIM3_CH2 GPIO Configuration for BOARD_GPIO_MTR_CTRL */
    LL_GPIO_SetPinMode(BOARD_GPIO_MTR_CTRL_PORT, BOARD_GPIO_MTR_CTRL_PIN, LL_GPIO_MODE_ALTERNATE);
    LL_GPIO_SetPinPull(BOARD_GPIO_MTR_CTRL_PORT, BOARD_GPIO_MTR_CTRL_PIN, LL_GPIO_PULL_DOWN);
    LL_GPIO_SetPinSpeed(BOARD_GPIO_MTR_CTRL_PORT, BOARD_GPIO_MTR_CTRL_PIN, LL_GPIO_SPEED_FREQ_HIGH);
    LL_GPIO_SetAFPin_0_7(BOARD_GPIO_MTR_CTRL_PORT, BOARD_GPIO_MTR_CTRL_PIN, LL_GPIO_AF_2);

    /* Direction GPIO */
    LL_GPIO_SetPinMode(BOARD_GPIO_MTR_DIR_PORT, BOARD_GPIO_MTR_DIR_PIN, LL_GPIO_MODE_OUTPUT);
    LL_GPIO_SetPinOutputType(BOARD_GPIO_MTR_DIR_PORT, BOARD_GPIO_MTR_DIR_PIN, LL_GPIO_OUTPUT_PUSHPULL);
    LL_GPIO_SetPinSpeed(BOARD_GPIO_MTR_DIR_PORT, BOARD_GPIO_MTR_DIR_PIN, LL_GPIO_SPEED_FREQ_LOW);
  }
}
