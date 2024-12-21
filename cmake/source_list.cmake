message("source_list.cmake")

set(source_list ${source_list}
${PROJ_PATH}/src/main.c
${PROJ_PATH}/src/startup_stm32l552xx.s
${PROJ_PATH}/src/bsp/system/system.c
)
