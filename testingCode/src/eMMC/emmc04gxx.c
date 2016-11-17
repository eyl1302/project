#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <strings.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/mman.h>
#include <fcntl.h>
#include "common.h"
#include "system.h"
#include "emmc04gxx.h"

//	Variable

//	Function
u8  fnEMMC04GxxInit(void);
u8	fnEMMC04GxxVerify(u8 mode);
u8  fnEMMC04GxxFunRead(u8 mode, char *str);
u8  fnEMMC04GxxFunWrite(u8 mode, char *str);

//	EMMC04Gxx Access Function
EMMC_FUNC_T emmc04gxx =
{
	fnEMMC04GxxInit,			//	Initial Function
	fnEMMC04GxxVerify,			//	Self Test
	fnEMMC04GxxFunRead,			//	Read Function
	fnEMMC04GxxFunWrite,		//	Write Function
};

/*
 * date: 2016/09/13
 * modify : hk
 * description: Initial EMMC04Gxx Function
 */
u8	fnEMMC04GxxInit(void)
{
	return HK_SUCCESS;
}
//------------------------------------------------

/*
 * date: 2016/09/13
 * modify : hk
 * description: Self Test Function
 */
u8	fnEMMC04GxxVerify(u8 mode)
{
	u8		resut;
	char 	buf[] = "1234567890ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyzzyxwvutsrqponmlkjihgfedcbaZYXWVUTSRQPONMLKJIHGFEDCBA0987654321";

	//	Create eMMC Verify File
	if(fnSystemSaveTxt(buf, EMMC_SFILE) == HK_FAIL)
		return HK_FAIL;

	if(mode)	//	Low Level
		resut = fnEMMCVerifyLowLevel();
	else		//	Quick
		resut = fnEMMCVerifyQuick();

	return resut;
}
//------------------------------------------------

/*
 * date: 2016/09/13
 * modify : hk
 * description: Read Function
 */
u8 fnEMMC04GxxFunRead(u8 mode, char *str)
{
	u8   result;
	char cmd[128];

	//	Mount emmc
	sprintf(cmd, "mount %s /mnt", EMMC_DEV);

	if(fnSystemCommandVerify(cmd) == HK_FAIL)
		return HK_FAIL;

	//	Move file from emmc
	printf("\n  ### hk_dbg, %s : read file from emmc......", __FUNCTION__);

	sprintf(cmd, "mv -f /mnt/%s ./", EMMC_SFILE);

	if(fnSystemCommandVerify(cmd) == HK_FAIL)
	{
		printf("fail!!\n");

		result = HK_FAIL;
	}
	else
		printf("ok!!\n");

	//	unmount emmc
	sprintf(cmd, "umount /mnt");

	if(fnSystemCommandVerify(cmd) == HK_FAIL)
		return HK_FAIL;

	//	Read Data from File
	if(fnSystemReadTxt(str, EMMC_SFILE) == HK_FAIL)
		result = HK_FAIL;
	else
		result = HK_SUCCESS;

	//	Remove Text File
	sprintf(cmd, "rm -f ./%s", EMMC_SFILE);

	if(fnSystemCommandVerify(cmd) == HK_FAIL)
		return HK_FAIL;

	return result;
}
//------------------------------------------------

/*
 * date: 2016/09/13
 * modify : hk
 * description: Write Function
 */
u8 fnEMMC04GxxFunWrite(u8 mode, char *str)
{
	u8	 result;
	char cmd[128];

	//	Create eMMC Verify File
	if(fnSystemSaveTxt(str, EMMC_SFILE) == HK_FAIL)
		return HK_FAIL;

	//	Mount emmc
	sprintf(cmd, "mount %s /mnt", EMMC_DEV);

	if(fnSystemCommandVerify(cmd) == HK_FAIL)
		return HK_FAIL;

	//	Move file to emmc
	printf("\n  ### hk_dbg, %s : write file to emmc......", __FUNCTION__);

	sprintf(cmd, "mv -f ./%s /mnt/", EMMC_SFILE);

    if(fnSystemCommandVerify(cmd) == HK_FAIL)
    {
    	printf("fail!!\n");

		result =  HK_FAIL;
	}
	else
	{
		printf("ok!!\n");

		result =  HK_SUCCESS;
	}

	//	Unmount emmc
	sprintf(cmd, "umount /mnt");

	if(fnSystemCommandVerify(cmd) == HK_FAIL)
		return HK_FAIL;

	return result;
}
//------------------------------------------------
