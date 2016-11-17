#ifndef _PMIC_COMM_H_
#define _PMIC_COMM_H_

#include "common.h"

u8   fnPMICRead(char* bus, u8 addr, u8 reg, u8 *buf, u8 len);
u8   fnPMICWrite(char* bus, u8 addr, u8 reg, u8 *buf, u8 len);

#endif	//	_PMIC_COMM_H_

                          