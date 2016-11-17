#ifndef _SDRAM_QUEUE_H_
#define _SDRAM_QUEUE_H_

#include "common.h"

#define SDRAM_QUEUE_SIZE	20

typedef enum
{
	DQ_AUTOTEST = 1,		//	Auto Test

} SDRAM_QUEUE_CMD_T;

typedef struct
{
	u8 	 cmd;				//	SDRAM_QUEUE_CMD_T

} SDRAM_QUQEUE_T;

//	Function
void fnSDRAMQueueInit(void);
void fnSDRAMQueuePush(SDRAM_QUQEUE_T sdram);
u8   fnSDRAMQueuePop(SDRAM_QUQEUE_T *sdram);
void fnSDRAMQueueFree(void);

#endif	//	_GSENSOR_QUEUE_H_
