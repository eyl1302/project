#ifndef _MT41KXX_H_
#define _MT41KXX_H_

#include "common.h"
#include "sdram.h"
#include "sdramComm.h"

#define MT41KXX_DEV						"/dev/mem"

#define CONFIG_SYS_MEMTEST_START       	0x10000000
#define CONFIG_SYS_MEMTEST_END         	0xFFFFFFFF
#define CONFIG_SYS_MEMTEST_SCRATCH     	0x10800000

extern SDRAM_FUNC_T mt41kxx;

#endif	//	_MT41KXX_H_
