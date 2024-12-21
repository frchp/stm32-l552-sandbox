message("include_list.cmake")

set(include_list ${include_list}
${PROJ_PATH}/src/app/intensity_sensor
${PROJ_PATH}/src/app/motor_controller
${PROJ_PATH}/src/app/order_handler
${PROJ_PATH}/src/app/speed_sensor
${PROJ_PATH}/src/bsp/adc
${PROJ_PATH}/src/bsp/clock
${PROJ_PATH}/src/bsp/gpio
${PROJ_PATH}/src/bsp/system
${PROJ_PATH}/src/bsp/timer_counter
${PROJ_PATH}/src/bsp/timer_pwm
${PROJ_PATH}/src/tasks
${PROJ_PATH}/src
${PROJ_PATH}/inc
# vendor
${PROJ_PATH}/vendor/Drivers/CMSIS/Core/Include
${PROJ_PATH}/vendor/Drivers/CMSIS/Device/ST/STM32L5xx/Include
${PROJ_PATH}/vendor/STM32L5xx_HAL_Driver/Inc
)
