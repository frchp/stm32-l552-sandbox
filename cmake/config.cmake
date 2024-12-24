message("config.cmake")

set(ADC_NB_SIGNALS 1)
configure_file(${PROJ_PATH}/src/bsp/adc/adc_config.h.in ${CMAKE_BINARY_DIR}/adc_config.h)

set(TIMER_COUNTER_NB_PULSES 2)
configure_file(${PROJ_PATH}/src/bsp/timer_counter/timer_counter_config.h.in ${CMAKE_BINARY_DIR}/timer_counter_config.h)

set(UART_MAX_SIZE 12)
configure_file(${PROJ_PATH}/src/bsp/uart/uart_config.h.in ${CMAKE_BINARY_DIR}/uart_config.h)

set(INTENSITY_SENSOR_GAIN 10) # TODO : adjust values
set(INTENSITY_SENSOR_SHUNT_IN_MILLIOHMS 30) # TODO : adjust values
set(INTENSITY_SENSOR_VDDA 3300)
configure_file(${PROJ_PATH}/src/app/intensity_sensor/intensity_sensor_config.h.in ${CMAKE_BINARY_DIR}/intensity_sensor_config.h)

set(SPEED_SENSOR_PULSES_PER_REV 4)
configure_file(${PROJ_PATH}/src/app/speed_sensor/speed_sensor_config.h.in ${CMAKE_BINARY_DIR}/speed_sensor_config.h)
