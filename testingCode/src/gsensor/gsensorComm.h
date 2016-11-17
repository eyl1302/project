#ifndef _GSENSOR_COMMON_H_
#define _GSENSOR_COMMON_H_

#include "common.h"

u8   fnGSensorRead(char* bus, u8 addr, u8 reg, u8 *buf, u8 len);
u8   fnGSensorWrite(char* bus, u8 addr, u8 reg, u8 *buf, u8 len);

#endif	//	_GSENSOR_COMMON_H_
