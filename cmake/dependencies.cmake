include(FetchContent)

message("dependencies.cmake")

list(APPEND CMAKE_MESSAGE_INDENT "|")

message("cube-l5")
FetchContent_Declare(
  cube-l5
  GIT_REPOSITORY https://github.com/STMicroelectronics/STM32CubeL5.git
  GIT_TAG v1.5.1
)
FetchContent_makeAvailable(cube-l5)

message("FreeRTOS not fetched")

list(POP_BACK CMAKE_MESSAGE_INDENT)