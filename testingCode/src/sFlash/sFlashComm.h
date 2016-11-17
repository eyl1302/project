#ifndef _SFLASH_COMM_H_
#define _SFLASH_COMM_H_

#include "common.h"
#include "sFlash.h"

u8 fnSFlashRead(char* dev, u8 *cmd, u8 *buf);
u8 fnSFlashWrite(char* dev, u8 addr, u8 reg, u8 *buf, u8 len);

#endif	//	_SFLASH_COMM_H_
