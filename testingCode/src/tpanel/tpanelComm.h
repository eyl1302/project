#ifndef _TPANEL_COMM_H_
#define _TPANEL_COMM_H_

#include "common.h"

u8   fnTPanelRead(char* bus, u8 addr, u8 reg, u8 *buf, u8 len);
u8   fnTPanelWrite(char* bus, u8 addr, u8 reg, u8 *buf, u8 len);

#endif	//	_TPANEL_COMM_H_

                          