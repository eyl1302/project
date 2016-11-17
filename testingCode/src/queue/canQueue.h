#ifndef _CAN_QUEUE_H_
#define _CAN_QUEUE_H_

#include "common.h"

#define CAN_QUEUE_SIZE	20

typedef enum
{
	CQ_AUTOTEST = 1,	//	Auto Test

} CAN_QUEUE_CMD_T;

typedef struct
{
	u8 	cmd;			//	CAN_QUEUE_CMD_T

}	CAN_QUQEUE_T;

//	Function
void fnCANQueueInit(void);
void fnCANQueuePush(CAN_QUQEUE_T can);
u8   fnCANQueuePop(CAN_QUQEUE_T *can);
void fnCANQueueFree(void);

#endif	//	_CAN_QUEUE_H_
