#ifndef _SYSTEM_QUEUE_H_
#define _SYSTEM_QUEUE_H_

#include "common.h"

#define SYSTEM_QUEUE_SIZE	20
#define SYSTEM_BUF_LEN		32

typedef enum
{
	SC_AUTOTEST = 1,				//	Start to Auto Test
	SC_REPLY,						//	Get Reply From Function Module

} SYSTEM_CMD_T;

typedef struct
{
	u8 	 cmd;						//	SYSTEM_CMD_T
	u8   func;						//	SYSTEM_COMMAND_T
	u8	 reply;						//	HK_TEST_T

}	SYSTEM_QUQEUE_T;

//	Function
void fnSystemQueueInit(void);
void fnSystemQueuePush(SYSTEM_QUQEUE_T system);
u8   fnSystemQueuePop(SYSTEM_QUQEUE_T *system);
void fnSystemQueueFree(void);

#endif	//	_SYSTEM_QUEUE_H_
