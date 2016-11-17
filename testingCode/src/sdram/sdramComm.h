#ifndef _SDRAM_COMM_H_
#define _SDRAM_COMM_H_

#include "common.h"

u8 fnSDRAMTestAlt(u64* buf, u64* dummy, u64 start, u64 length);
u8 fnSDRAMTestQuick(u64 *buf, u64 sAddr, u64 length, u64 pattern, s32 iteration);

#endif	//	_SDRAM_COMM_H_
