message("freertos.cmake")

set(FREERTOS_PORT GCC_ARM_CM33_NTZ_NONSECURE CACHE STRING "")

add_library(freertos_config INTERFACE)
target_include_directories(freertos_config SYSTEM INTERFACE ${PROJ_PATH}/inc)
target_compile_definitions(freertos_config
      INTERFACE
      projCOVERAGE_TEST=0)