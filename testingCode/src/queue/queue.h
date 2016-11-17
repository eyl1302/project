#ifndef _QUEUE_H_
#define _QUEUE_H_

#include "common.h"

typedef enum
{
	QID_RTC = 1,
	QID_BATTERY,
	QID_AUDIO,
	QID_GSENSOR,
	QID_SFLASH,
	QID_SDRAM,
	QID_EMMC,
	QID_USB,
	QID_TPANEL,
	QID_PMIC,
	QID_CAN,
	QID_SYSTEM,

}	QUEUE_ID_T;

#define QUEUE_STR(id)							\
	   ((id == QID_RTC)?"RTC Queue": 			\
	   ((id == QID_BATTERY)?"Battery Queue": 	\
	   ((id == QID_AUDIO)?"Audio Queue": 		\
	   ((id == QID_GSENSOR)?"GSensor Queue": 	\
	   ((id == QID_SFLASH)?"SPI-Flash Queue": 	\
	   ((id == QID_SDRAM)?"SDRAM Queue": 		\
	   ((id == QID_EMMC)?"eMMC Queue": 			\
	   ((id == QID_USB)?"USB Queue": 			\
	   ((id == QID_TPANEL)?"Touch Panel Queue":	\
	   ((id == QID_PMIC)?"PMIC Queue":	\
	   ((id == QID_CAN)?"CAN Queue":	\
	   ((id == QID_SYSTEM)?"System Queue":"Unknow"))))))))))))

//	Function
void* fnQueueInit(u8 len, u8 size, u8 id) ;
void fnQueuePush(void* dest, void* src, u8 size);
void fnQueuePop(void* src, void* dest, u8 size);
void fnQueueFree(void* queue);

#endif	//	_QUEUE_H_
