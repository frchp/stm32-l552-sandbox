message("include_list.cmake")

set(include_list ${include_list}
${PROJ_PATH}/src
${PROJ_PATH}/src/clock
${PROJ_PATH}/src/gpio
${PROJ_PATH}/src/system
${PROJ_PATH}/src/timer
${PROJ_PATH}/inc
# vendor
${PROJ_PATH}/vendor/Drivers/CMSIS/Core/Include
${PROJ_PATH}/vendor/Drivers/CMSIS/Device/ST/STM32L5xx/Include
)
