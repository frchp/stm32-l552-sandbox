#ifndef _DIRECTION_CONTROLLER_H_
#define _DIRECTION_CONTROLLER_H_

typedef enum
{
  MTR_DIR_CW,
  MTR_DIR_CCW
} MotorDirection_t;

/**
  @brief Setup DirectionController.
 */
void DirectionController_Init(void);

/**
  @brief Go to direction Clock Wise
 */
void DirectionController_GoCW(void);

/**
  @brief Go to direction Counter Clock Wise
 */
void DirectionController_GoCCW(void);

#endif // _DIRECTION_CONTROLLER_H_