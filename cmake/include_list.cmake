message("include_list.cmake")

set(include_list ${include_list}
${PROJ_PATH}/src/app
${PROJ_PATH}/src/app/direction_controller
${PROJ_PATH}/src/app/intensity_sensor
${PROJ_PATH}/src/app/motor_controller
${PROJ_PATH}/src/app/order_handler
${PROJ_PATH}/src/app/speed_sensor
${PROJ_PATH}/src/bsp
${PROJ_PATH}/src/bsp/adc
${PROJ_PATH}/src/bsp/interrupts
${PROJ_PATH}/src/bsp/system
${PROJ_PATH}/src/bsp/timer_counter
${PROJ_PATH}/src/bsp/timer_pwm
${PROJ_PATH}/src/bsp/uart
${PROJ_PATH}/src/bsp/watchdog
${PROJ_PATH}/src/tasks
${PROJ_PATH}/src
${PROJ_PATH}/inc
# tasks
${PROJ_PATH}/src/tasks
${PROJ_PATH}/src/tasks/media
${PROJ_PATH}/src/tasks/motor
# vendor
${PROJ_PATH}/vendor/Drivers/CMSIS/Core/Include
${PROJ_PATH}/vendor/Drivers/CMSIS/Device/ST/STM32L5xx/Include
${PROJ_PATH}/vendor/STM32L5xx_HAL_Driver/Inc
# config files
${CMAKE_BINARY_DIR}
)
