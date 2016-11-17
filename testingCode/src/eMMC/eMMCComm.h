#ifndef _AUDIO_COMM_H_
#define _AUDIO_COMM_H_

#include "common.h"
#include "eMMC.h"

//	eMMC Device
#define EMMC_DEV					"/dev/mmcblk0p1"

//	eMMC Testing File
#define EMMC_SFILE					"src.txt"
#define EMMC_CFILE					"cmp.txt"

u8 fnEMMCVerifyQuick(void);
u8 fnEMMCVerifyLowLevel(void);
u8 fnEMMCLLRead(char *ifile, char *ofile, u16 skip, u16 ibs);
u8 fnEMMCLLWrite(char *ifile, char *ofile, u16 seek, u16 obs);
u8 fnEMMCQKRead(void);
u8 fnEMMCQKWrite(void);


#endif	//	_AUDIO_COMM_H_
