message("freertos.cmake")

set(FREERTOS_PORT GCC_ARM_CM33_NTZ_NONSECURE CACHE STRING "")

add_library(freertos_config INTERFACE)
target_include_directories(freertos_config SYSTEM INTERFACE 
${PROJ_PATH}/inc #FreeRTOSConfig.h
# Idle task functions
${PROJ_PATH}/src/tasks/idle
# Arm drivers
${PROJ_PATH}/vendor/Drivers/CMSIS/Device/ST/STM32L5xx/Include
${PROJ_PATH}/vendor/Drivers/CMSIS/Core/Include)

target_compile_definitions(freertos_config
      INTERFACE
      projCOVERAGE_TEST=0)
