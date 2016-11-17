#ifndef _BATTERY_COMM_H_
#define _BATTERY_COMM_H_

#include "common.h"
#include "battery.h"

u8 fnBatteryRead(char* bus, u8 addr, u8 reg, s32 *buf, u8 len);
u8 fnBatteryWrite(char* bus, u8 addr, u8 reg, u8 *buf, u8 len);

#endif	//	_BATTERY_COMM_H_
