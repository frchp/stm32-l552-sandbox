message("config.cmake")

set(ADC_NB_SIGNALS 2)
configure_file(${PROJ_PATH}/src/bsp/adc/adc_config.h.in ${CMAKE_BINARY_DIR}/adc_config.h)

set(TIMER_COUNTER_NB_PULSES 10)
configure_file(${PROJ_PATH}/src/bsp/timer_counter/timer_counter_config.h.in ${CMAKE_BINARY_DIR}/timer_counter_config.h)

set(UART_MAX_SIZE 12)
configure_file(${PROJ_PATH}/src/bsp/uart/uart_config.h.in ${CMAKE_BINARY_DIR}/uart_config.h)
