#ifndef _SFLASH_QUEUE_H_
#define _SFLASH_QUEUE_H_

#include "common.h"

#define SFLASH_QUEUE_SIZE	20

typedef enum
{
	SQ_AUTOTEST = 1,	//	Auto Test

} SFLASH_QUEUE_CMD_T;

typedef struct
{
	u8 	cmd;			//	SFLASH_QUEUE_CMD_T

}	SFLASH_QUQEUE_T;

//	Function
void fnSFlashQueueInit(void);
void fnSFlashQueuePush(SFLASH_QUQEUE_T sflash);
u8   fnSFlashQueuePop(SFLASH_QUQEUE_T *sflash);
void fnSFlashQueueFree(void);

#endif	//	_SFLASH_QUEUE_H_
