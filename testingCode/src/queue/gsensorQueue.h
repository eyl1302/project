#ifndef _GSENSOR_QUEUE_H_
#define _GSENSOR_QUEUE_H_

#include "common.h"

#define GSENSOR_QUEUE_SIZE	20

typedef enum
{
	GQ_AUTOTEST = 1,		//	Auto Test

} GSENSOR_QUEUE_CMD_T;

typedef struct
{
	u8 	 cmd;				//	GSENSOR_QUEUE_CMD_T

} GSENSOR_QUQEUE_T;

//	Function
void fnGSensorQueueInit(void);
void fnGSensorQueuePush(GSENSOR_QUQEUE_T gsensor);
u8   fnGSensorQueuePop(GSENSOR_QUQEUE_T *gsensor);
void fnGSensorQueueFree(void);

#endif	//	_GSENSOR_QUEUE_H_
