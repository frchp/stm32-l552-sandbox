#include "app.h"
#include "direction_controller.h"
#include "intensity_sensor.h"
#include "speed_sensor.h"
#include "motor_controller.h"
#include "order_handler.h"

/**
  @brief Setup the application.
 */
void App_Init(void)
{
  DirectionController_Init();
  IntensitySensor_Init();
  SpeedSensor_Init();
  MotorController_Init();
  OrderHandler_Init();
}
