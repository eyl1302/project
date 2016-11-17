#ifndef _EMMC_QUEUE_H_
#define _EMMC_QUEUE_H_

#include "common.h"

#define EMMC_QUEUE_SIZE	20

typedef enum
{
	EQ_AUTOTEST = 1,	//	Auto Test

} EMMC_QUEUE_CMD_T;

typedef struct
{
	u8 	cmd;			//	EMMC_QUEUE_CMD_T

}	EMMC_QUQEUE_T;

//	Function
void fnEMMCQueueInit(void);
void fnEMMCQueuePush(EMMC_QUQEUE_T emmc);
u8   fnEMMCQueuePop(EMMC_QUQEUE_T *emmc);
void fnEMMCQueueFree(void);

#endif	//	_RTC_QUEUE_H_
