message("source_list.cmake")

set(source_list ${source_list}
${PROJ_PATH}/src/main.c
${PROJ_PATH}/src/startup_stm32l552xx.s
${PROJ_PATH}/src/clock/clock.c
${PROJ_PATH}/src/gpio/gpio.c
${PROJ_PATH}/src/system/system.c
${PROJ_PATH}/src/timer/timer.c
)
