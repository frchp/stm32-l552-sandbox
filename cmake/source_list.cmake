message("source_list.cmake")

set(source_list ${source_list}
${PROJ_PATH}/src/main.c
${PROJ_PATH}/src/startup_stm32l552xx.s
${PROJ_PATH}/src/bsp/system/system.c
# vendor
${PROJ_PATH}/vendor/STM32L5xx_HAL_Driver/Src/stm32l5xx_ll_adc.c
${PROJ_PATH}/vendor/STM32L5xx_HAL_Driver/Src/stm32l5xx_ll_adc.c
${PROJ_PATH}/vendor/STM32L5xx_HAL_Driver/Src/stm32l5xx_ll_comp.c
${PROJ_PATH}/vendor/STM32L5xx_HAL_Driver/Src/stm32l5xx_ll_crc.c
${PROJ_PATH}/vendor/STM32L5xx_HAL_Driver/Src/stm32l5xx_ll_crs.c
${PROJ_PATH}/vendor/STM32L5xx_HAL_Driver/Src/stm32l5xx_ll_dac.c
${PROJ_PATH}/vendor/STM32L5xx_HAL_Driver/Src/stm32l5xx_ll_dma.c
${PROJ_PATH}/vendor/STM32L5xx_HAL_Driver/Src/stm32l5xx_ll_exti.c
${PROJ_PATH}/vendor/STM32L5xx_HAL_Driver/Src/stm32l5xx_ll_fmc.c
${PROJ_PATH}/vendor/STM32L5xx_HAL_Driver/Src/stm32l5xx_ll_gpio.c
${PROJ_PATH}/vendor/STM32L5xx_HAL_Driver/Src/stm32l5xx_ll_i2c.c
${PROJ_PATH}/vendor/STM32L5xx_HAL_Driver/Src/stm32l5xx_ll_icache.c
${PROJ_PATH}/vendor/STM32L5xx_HAL_Driver/Src/stm32l5xx_ll_lptim.c
${PROJ_PATH}/vendor/STM32L5xx_HAL_Driver/Src/stm32l5xx_ll_lpuart.c
${PROJ_PATH}/vendor/STM32L5xx_HAL_Driver/Src/stm32l5xx_ll_opamp.c
${PROJ_PATH}/vendor/STM32L5xx_HAL_Driver/Src/stm32l5xx_ll_pka.c
${PROJ_PATH}/vendor/STM32L5xx_HAL_Driver/Src/stm32l5xx_ll_pwr.c
${PROJ_PATH}/vendor/STM32L5xx_HAL_Driver/Src/stm32l5xx_ll_rcc.c
${PROJ_PATH}/vendor/STM32L5xx_HAL_Driver/Src/stm32l5xx_ll_rng.c
${PROJ_PATH}/vendor/STM32L5xx_HAL_Driver/Src/stm32l5xx_ll_rtc.c
${PROJ_PATH}/vendor/STM32L5xx_HAL_Driver/Src/stm32l5xx_ll_sdmmc.c
${PROJ_PATH}/vendor/STM32L5xx_HAL_Driver/Src/stm32l5xx_ll_spi.c
${PROJ_PATH}/vendor/STM32L5xx_HAL_Driver/Src/stm32l5xx_ll_tim.c
${PROJ_PATH}/vendor/STM32L5xx_HAL_Driver/Src/stm32l5xx_ll_ucpd.c
${PROJ_PATH}/vendor/STM32L5xx_HAL_Driver/Src/stm32l5xx_ll_usart.c
${PROJ_PATH}/vendor/STM32L5xx_HAL_Driver/Src/stm32l5xx_ll_usb.c
${PROJ_PATH}/vendor/STM32L5xx_HAL_Driver/Src/stm32l5xx_ll_utils.c
)
