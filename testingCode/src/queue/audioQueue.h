#ifndef _AUDIO_QUEUE_H_
#define _AUDIO_QUEUE_H_

#include "common.h"

#define AUDIO_QUEUE_SIZE	20

typedef enum
{
	AQ_AUTOTEST = 1,	//	Auto Test

} AUDIO_QUEUE_CMD_T;

typedef struct
{
	u8 	 cmd;						//	AUDIO_QUEUE_CMD_T

} AUDIO_QUQEUE_T;

//	Function
void fnAudioQueueInit(void);
void fnAudioQueuePush(AUDIO_QUQEUE_T audio);
u8   fnAudioQueuePop(AUDIO_QUQEUE_T *audio);
void fnAudioQueueFree(void);

#endif	//	_AUDIO_QUEUE_H_
