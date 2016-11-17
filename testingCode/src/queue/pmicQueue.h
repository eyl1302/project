#ifndef _PMIC_QUEUE_H_
#define _PMIC_QUEUE_H_

#include "common.h"

#define PMIC_QUEUE_SIZE	20

typedef enum
{
	PQ_AUTOTEST = 1,	//	Auto Test

} PMIC_QUEUE_CMD_T;

typedef struct
{
	u8 	cmd;			//	PMIC_QUEUE_CMD_T

}	PMIC_QUQEUE_T;

//	Function
void fnPMICQueueInit(void);
void fnPMICQueuePush(PMIC_QUQEUE_T pmic);
u8   fnPMICQueuePop(PMIC_QUQEUE_T *pmic);
void fnPMICQueueFree(void);

#endif	//	_PMIC_QUEUE_H_
