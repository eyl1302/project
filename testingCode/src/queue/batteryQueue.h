#ifndef _BATTERY_QUEUE_H_	//	Gas Gauge
#define _BATTERY_QUEUE_H_

#include "common.h"

#define BATTERY_QUEUE_SIZE	20
#define BATTERY_BUF_LEN		32

typedef enum
{
	BQ_AUTOTEST = 1,	//	Auto Test

} BATTERY_QUEUE_CMD_T;

typedef struct
{
	u8 	 cmd;						//	BATTERY_QUEUE_CMD_T
	u32  temp;						//	Temperature
	u32  cap;						//	Capacity
	s32	 cur;						//	Average Current
	u32	 vol;						//	Charge Voltage
	char status[BATTERY_BUF_LEN];	//	Status of Charge

}	BATTERY_QUQEUE_T;

//	Function
void fnBatteryQueueInit(void);
void fnBatteryQueuePush(BATTERY_QUQEUE_T battery);
u8   fnBatteryQueuePop(BATTERY_QUQEUE_T *battery);
void fnBatteryQueueFree(void);

#endif	//	_BATTERY_QUEUE_H_
