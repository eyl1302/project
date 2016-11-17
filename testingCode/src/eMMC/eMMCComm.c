#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <strings.h>
#include <unistd.h>
#include <fcntl.h>
#include <time.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/ioctl.h>
#include "i2c-dev.h"
#include "common.h"
#include "system.h"
#include "eMMCComm.h"

/*
 * date: 2016/09/20
 * modify : hk
 * description: eMMC Quick Verify Function
 */
u8 fnEMMCVerifyQuick(void)
{
	char cmd[128];

	//	mount emmc
	sprintf(cmd, "mount %s /mnt", EMMC_DEV);

	if(fnSystemCommandVerify(cmd) == HK_FAIL)
		return HK_FAIL;

	//	copy file to emmc
	printf("\n  ### hk_dbg, %s : write file to emmc......", __FUNCTION__);

	sprintf(cmd, "cp -f ./%s /mnt", EMMC_SFILE);

    if(fnSystemCommandVerify(cmd) == HK_FAIL)
    {
    	printf("fail!!\n");

		return HK_FAIL;
	}

	printf("ok!!\n");

    //	Wait for a while
	usleep(EMMC_SLEEP_TIME);

	//	copy file from emmc
	printf("\n  ### hk_dbg, %s : read file from emmc......", __FUNCTION__);

	sprintf(cmd, "cp -f /mnt/%s ./%s", EMMC_SFILE, EMMC_CFILE);

	if(fnSystemCommandVerify(cmd) == HK_FAIL)
	{
		printf("fail!!\n");

		return HK_FAIL;
	}

	printf("ok!!\n");

	//	compare
	printf("\n  ### hk_dbg, %s : compare two files......", __FUNCTION__);

	sprintf(cmd, "diff ./%s ./%s", EMMC_SFILE, EMMC_CFILE);

	if(fnSystemCommandVerify(cmd) == HK_FAIL)
	{
		printf("fail!!\n");
		return HK_FAIL;
	}

	printf("ok!!\n");

	//	remove file
	sprintf(cmd, "rm -f /mnt/%s", EMMC_SFILE);

	if(fnSystemCommandVerify(cmd) == HK_FAIL)
		return HK_FAIL;

	sprintf(cmd, "rm -f ./%s && rm -f ./%s", EMMC_SFILE, EMMC_CFILE);

	if(fnSystemCommandVerify(cmd) == HK_FAIL)
		return HK_FAIL;

	//	unmount emmc
	sprintf(cmd, "umount /mnt");

	if(fnSystemCommandVerify(cmd) == HK_FAIL)
		return HK_FAIL;

	return HK_SUCCESS;
}
//-----------------------------------------------------

/*
 * date: 2016/09/20
 * modify : hk
 * description: eMMC Low Level Verify Function
 */
u8 fnEMMCVerifyLowLevel(void)
{
	u8 	result;
	u16 seek = 0;

	//	Write to eMMC
	fnEMMCLLWrite(EMMC_DEV, EMMC_SFILE, seek, 124);

	//	Wait for a while
	usleep(EMMC_SLEEP_TIME);

	//	Read from eMMC
	fnEMMCLLRead(EMMC_DEV, EMMC_CFILE, 0, 124);

	//	Compare

	result = HK_SUCCESS;

	return result;
}
//-----------------------------------------------------

/*
 * date: 2016/09/20
 * modify : hk
 * description: eMMC Low Level Read Function
 */
u8 fnEMMCLLRead(char *ifile, char *ofile, u16 skip, u16 ibs)
{
	char cmd[128];

	//	Read data from eMMC
	sprintf(cmd, "dd if=%s of=%s skip=%d ibs=%d", ifile, ofile, skip, ibs);

	system(cmd);

	return HK_SUCCESS;
}
//-----------------------------------------------------

/*
 * date: 2016/09/20
 * modify : hk
 * description: eMMC Low Level Write Function
 */
u8 fnEMMCLLWrite(char *ifile, char *ofile, u16 seek, u16 obs)
{
	char cmd[128];

	//	Write data to eMMC
	sprintf(cmd, "dd if=%s of=%s seek=%d obs=%d", ifile, ofile, seek, obs);

	system(cmd);

	return HK_SUCCESS;
}
//-----------------------------------------------------

/*
 * date: 2016/09/21
 * modify : hk
 * description: eMMC Quick Read Function
 */
u8 fnEMMCQKRead(void)
{
	return HK_SUCCESS;
}
//-----------------------------------------------------

/*
 * date: 2016/09/21
 * modify : hk
 * description: eMMC Quick Write Function
 */
u8 fnEMMCQKWrite(void)
{
	return HK_SUCCESS;
}
//-----------------------------------------------------
