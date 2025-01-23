#ifndef _MEDIA_TASK_H_
#define _MEDIA_TASK_H_

#include "FreeRTOS.h"
#include "task.h"

/* Media task configuration */
#define MEDIA_TASK_PRIORITY     (2u)
#define MEDIA_TASK_STACK_SIZE   (128u)

extern TaskHandle_t gbl_sMediaTaskHandle;
extern StackType_t gbl_sStackMedia[MEDIA_TASK_STACK_SIZE];
extern StaticTask_t gbl_sTCBMedia;

void MediaTask_OnReception(void);

void MediaTask(void *arg_pvParameters);

#endif // _MEDIA_TASK_H_
