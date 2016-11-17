#ifndef _TPANEL_QUEUE_H_
#define _TPANEL_QUEUE_H_

#include "common.h"

#define TPANEL_QUEUE_SIZE	20

typedef enum
{
	TQ_AUTOTEST = 1,	//	Auto Test

} TPANEL_QUEUE_CMD_T;

typedef struct
{
	u8 	cmd;			//	TPANEL_QUEUE_CMD_T

}	TPANEL_QUQEUE_T;

//	Function
void fnTPanelQueueInit(void);
void fnTPanelQueuePush(TPANEL_QUQEUE_T tpanel);
u8   fnTPanelQueuePop(TPANEL_QUQEUE_T *tpanel);
void fnTPanelQueueFree(void);

#endif	//	_TPANEL_QUEUE_H_
