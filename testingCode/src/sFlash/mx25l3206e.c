#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <strings.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/stat.h>
#include "common.h"
#include "system.h"
#include "mx25l3206e.h"

//	Function

//	Function Pointer
u8  fnMX25L3206Info(void);
u8  fnMX25L3206Verify(void);
u8  fnMX25L3206Read(void);
u8  fnMX25L3206Write(void);
u8  fnMX25L3206Erase(void);

SFLASH_FUNC_T	mx25l3206e =
{
	fnMX25L3206Info,
	fnMX25L3206Verify,
	fnMX25L3206Read,
	fnMX25L3206Write,
	fnMX25L3206Erase,
};

/*
 * date: 2016/08/03
 * modify : hk
 * description: MX25L3206E Read File
 */
u8 fnSFReadFile(char *name, u8 *buf, u32 len)
{
	FILE	*file;

	file = fopen(name, "r");

	if(file == NULL)
	{
		printf("\n  ### hk_dbg, %s : line:%d,  Read From Flash Fail~~\n", __FUNCTION__, __LINE__);

		return HK_FAIL;
	}

	if(fread(buf, 1, len, file) != len)
	{
		printf("\n  ### hk_dbg, %s : Empty Data~~\n", __FUNCTION__);

		fclose(file);

		return HK_FAIL;
	}

	fclose(file);

	return HK_SUCCESS;
}
//------------------------------------------------

/*
 * date: 2016/08/03
 * modify : hk
 * description: MX25L3206E Display Information
 */
u8  fnMX25L3206Info(void)
{
    printf("\n");
	printf("        |==========================================| \n");
	printf("        |          MX25L3206e Organization         | \n");
	printf("        |==========================================| \n");
	printf("        |  1 Blocks = 16 Secotrs                   | \n");
	printf("        |  1 Secotr = 4096 Bytes                   | \n");
	printf("        |  64 Blcoks = 64 * 16  * 4096 = 4M Bytes  | \n");
	printf("        |==========================================| \n");
	printf("        |  Block  |  Sector  |   Address Range     | \n");
	printf("        |==========================================| \n");
	printf("        |         |   1023   |  3FF000h | 3FFFFFh  | \n");
	printf("        |         ---------------------------------| \n");
	printf("        |    63   |   ...    |   ...    |   ...    | \n");
	printf("        |         ---------------------------------| \n");
	printf("        |         |   1008   |  3F0000h | 3F0FFFh  | \n");
	printf("        |==========================================| \n");
	printf("        |         |   1007   |  3EF000h | 3EFFFFh  | \n");
	printf("        |         ---------------------------------| \n");
	printf("        |    62   |   ...    |    ...   |   ...    | \n");
	printf("        |         ---------------------------------| \n");
	printf("        |         |   992    |  3E0000h | 3E0FFFh  | \n");
	printf("        |==========================================| \n");
	printf("        |         |   ...    |    ...   |   ...    | \n");
	printf("        |         ---------------------------------| \n");
	printf("        |   ...   |   ...    |    ...   |   ...    | \n");
	printf("        |         ---------------------------------| \n");
	printf("        |         |   ...    |    ...   |   ...    | \n");
	printf("        |==========================================| \n");
	printf("        |         |    15    |  00F000h | 00FFFFh  | \n");
	printf("        |         ---------------------------------| \n");
	printf("        |         |   ...    |    ...   |   ...    | \n");
	printf("        |         ---------------------------------| \n");
	printf("        |         |    3     |  003000h | 003FFFh  | \n");
	printf("        |    0    ---------------------------------| \n");
	printf("        |         |    2     |  002000h | 002FFFh  | \n");
	printf("        |         ---------------------------------| \n");
	printf("        |         |    1     |  001000h | 001FFFh  | \n");
	printf("        |         ---------------------------------| \n");
	printf("        |         |    0     |  000000h | 000FFFh  | \n");
    printf("        |==========================================| \n");
	printf("\n");


	//char cmd[128];
    //
	//sprintf(cmd, "cat /proc/mtd");
    //
	//system(cmd);

	return HK_SUCCESS;
}
//------------------------------------------------

/*
 * date: 2016/07/26
 * modify : hk
 * description: MX25L3206E Verify Function
 */
u8  fnMX25L3206Verify(void)
{
	u16     len;
	char 	cmd[128];
	char 	buf[]="abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789";

	len = strlen(buf);

	printf("\n");

	//	Erase Flash
	sprintf(cmd, "mtd_debug erase %s 0x0 0x%x", MX25L3206E_UCDEV, MX25L3206E_BLOCK_SIZE);

	system(cmd);

	//	Read From Flash
	printf("\n");

	sprintf(cmd, "mtd_debug read %s 0x0 0x%x %s", MX25L3206E_UBDEV, MTD_ACC_LEN, RSF_IMG);

	system(cmd);

	//	Check Empty Or Not
	{
		u32	i;
		u8  buf[MTD_ACC_LEN];

		if(fnSFReadFile(RSF_IMG, buf, MTD_ACC_LEN) == HK_FAIL)
			return HK_FAIL;

		for(i=0;i<MTD_ACC_LEN;i++)
		{
			if(buf[i] != 0xFF)	//	Clear Status
			{
				printf("\n  ### hk_dbg, %s : Erase Flash Fail!!\n", __FUNCTION__);

				return HK_FAIL;
			}
		}

		sprintf(cmd, "rm -f %s", RSF_IMG);

		system(cmd);
	}


	//	Write To Flash
	{
		sprintf(cmd, "echo %s > %s", buf, WSF_IMG);

		system(cmd);

		printf("\n");

		sprintf(cmd, "mtd_debug write %s 0x0 0x%x %s", MX25L3206E_UBDEV, len, WSF_IMG);

		system(cmd);
	}

	//	Read From Flash
	{
		printf("\n");

		sprintf(cmd, "mtd_debug read %s 0x0 0x%x %s", MX25L3206E_UBDEV, len, RSF_IMG);

		system(cmd);
	}

	//	Compare Write and Read File
	{
		u32 i;
		u8 sBuf[MTD_ACC_LEN];
		u8 dBuf[MTD_ACC_LEN];

		//	Write Serial Image
		if(fnSFReadFile(WSF_IMG, sBuf, len) == HK_FAIL)
		{
			printf("\n  ### hk_dbg, %s : Compare Fail~~\n", __FUNCTION__);

			return HK_FAIL;
		}

		//	Read Serial Image
		if(fnSFReadFile(RSF_IMG, dBuf, len) == HK_FAIL)
		{
			printf("\n  ### hk_dbg, %s : Compare Fail~~\n", __FUNCTION__);

			return HK_FAIL;
		}

		sscanf((char*)dBuf, "%[^ÿ]", dBuf);

		//	Compare
		for(i=0;i<len;i++)
		{
			if(sBuf[i] != dBuf[i])
			{
				printf("\n  ### hk_dbg, %s : Compare Different Fail~~\n", __FUNCTION__);

				return HK_FAIL;
			}
		}

		sprintf(cmd, "rm -f *.img");

		system(cmd);
	}

	return HK_SUCCESS;
}
//------------------------------------------------

/*
 * date: 2016/07/26
 * modify : hk
 * description: MX25L3206E Read Function
 */
u8  fnMX25L3206Read(void)
{
	s8	block;
	u32 addr;

	//	Block Select
	printf("\n  ### hk_dbg, %s : Please Select Address to Read(Block 0 ~ 63): ", __FUNCTION__);

	block = fnSystemInputString();

	if((block < MX25L3206E_BLOCK_MIN) || (block > MX25L3206E_BLOCK_MAX))
	{
		printf("\n  ### hk_dbg, %s : Out of Range, Please Try Agagin!!\n", __FUNCTION__);

		return HK_FAIL;
	}

	//	SPI Flash was divided into 2 equal size partition, block ragne from 0 ~ 31
	if(block >= FCDEV_START_BLCK)
		block -= FCDEV_START_BLCK;

	//	Start Address of Block
	addr = block * MX25L3206E_BLOCK_SIZE;

	//	Read
	{
		char cmd[SCEEN_STR_LEN];

		printf("\n");

		if(block < FCDEV_START_BLCK)
			sprintf(cmd, "mtd_debug read %s 0x%x 0x%x %s", MX25L3206E_UBDEV, addr, MTD_ACC_LEN, RSF_IMG);
		else
			sprintf(cmd, "mtd_debug read %s 0x%x 0x%x %s", MX25L3206E_FBDEV, addr, MTD_ACC_LEN, RSF_IMG);

		system(cmd);

		//	Display
		{
			u8 	buf[MTD_ACC_LEN];
			u8  str[MTD_ACC_LEN];

			memset(str, 0x00, MTD_ACC_LEN);

			fnSFReadFile(RSF_IMG, buf, MTD_ACC_LEN);

			sscanf((char*)buf, "%[^ÿ]", str);

			printf("\n  ### hk_dbg, %s : read '%s' from flash!\n", __FUNCTION__, str);
		}

		//	Delete
		printf("\n");

		sprintf(cmd, "rm -f %s ", RSF_IMG);

		system(cmd);
	}

	return HK_SUCCESS;
}
//------------------------------------------------

/*
 * date: 2016/07/26
 * modify : hk
 * description: MX25L3206E Write Function
 */
u8  fnMX25L3206Write(void)
{
	s8	block;
	u32 addr;

	//	Block Select
	printf("\n  ### hk_dbg, %s : Please Select Address to Write(Block 0 ~ 63): ", __FUNCTION__);

	block = fnSystemInputString();

	if((block < MX25L3206E_BLOCK_MIN) || (block > MX25L3206E_BLOCK_MAX))
	{
		printf("\n  ### hk_dbg, %s : Out of Range, Please Try Agagin!!\n", __FUNCTION__);

		return HK_FAIL;
	}

	//	SPI Flash was divided into 2 equal size partition, block ragne from 0 ~ 31
	if(block >= FCDEV_START_BLCK)
		block -= FCDEV_START_BLCK;

	//	Start Address of Block
	addr = block * MX25L3206E_BLOCK_SIZE;

	//	Write
	{
		u16  len;
		char str[SCEEN_STR_LEN];
		char cmd[SCEEN_STR_LEN];

		printf("\n  ### hk_dbg, %s : Please Input a short and consecutive string (length < 40): ", __FUNCTION__);

		//	Get String write to Flash
		scanf("\t %s", str);

		len = strlen(str);

		//printf("\n  ### hk_dbg, %s : str:%s, len:%d!!\n", __FUNCTION__, str, len);

		sprintf(cmd, "echo %s > %s", str, WSF_IMG);

		system(cmd);

		printf("\n  ### hk_dbg, %s : write '%s' to flash!!\n", __FUNCTION__, str);

		printf("\n");

		if(block < FCDEV_START_BLCK)
			sprintf(cmd, "mtd_debug write %s 0x%x 0x%x %s", MX25L3206E_UBDEV, addr, len, WSF_IMG);
		else
			sprintf(cmd, "mtd_debug write %s 0x%x 0x%x %s", MX25L3206E_FBDEV, addr, len, WSF_IMG);

		system(cmd);

		//	Delete
		printf("\n");

		sprintf(cmd, "rm -f %s ", WSF_IMG);

		system(cmd);
	}

	return HK_SUCCESS;
}
//------------------------------------------------


/*
 * date: 2016/07/26
 * modify : hk
 * description: MX25L3206E Erase Function
 */
u8  fnMX25L3206Erase(void)
{
	s8 		block;
	u8		result;
	u32 	addr;
	char 	cmd[128];

	printf("\n  ### hk_dbg, %s : Please Select Address to Erase(Block 0 ~ 63): ", __FUNCTION__);

	block  = fnSystemInputString();

	if((block < MX25L3206E_BLOCK_MIN) || (block > MX25L3206E_BLOCK_MAX))
	{
		printf("\n  ### hk_dbg, %s : Out of Range, Please Try Agagin!!\n", __FUNCTION__);

		return HK_FAIL;
	}

	//	SPI Flash was divided into 2 equal size partition, block ragne from 0 ~ 31
	if(block >= FCDEV_START_BLCK)
		block -= FCDEV_START_BLCK;

	//	Start Address of Block
	addr = block * MX25L3206E_BLOCK_SIZE;

	//	Erase Flash
	{
		printf("\n");

		if(block < FCDEV_START_BLCK)
			sprintf(cmd, "mtd_debug erase %s 0x%x 0x%x", MX25L3206E_UCDEV, addr, MX25L3206E_BLOCK_SIZE);
		else
			sprintf(cmd, "mtd_debug erase %s 0x%x 0x%x", MX25L3206E_FCDEV, addr, MX25L3206E_BLOCK_SIZE);

		system(cmd);
	}

	//	Read From Flash
	printf("\n");

	if(block < FCDEV_START_BLCK)
		sprintf(cmd, "mtd_debug read %s 0x%x 0x%x %s", MX25L3206E_UBDEV, addr, MTD_ACC_LEN, RSF_IMG);
	else
		sprintf(cmd, "mtd_debug read %s 0x%x 0x%x %s", MX25L3206E_FBDEV, addr, MTD_ACC_LEN, RSF_IMG);

	system(cmd);

	//	Check Empty Or Not
	{
		u32		i;
		u8  	buf[MTD_ACC_LEN];

		if(fnSFReadFile(RSF_IMG, buf, MTD_ACC_LEN) == HK_FAIL)
			return HK_FAIL;

		for(i=0;i<MTD_ACC_LEN;i++)
		{
			if(buf[i] != 0xFF)	//	Clear Status
			{
				printf("\n  ### hk_dbg, %s : Erase Flash Fail!!\n", __FUNCTION__);

				return HK_FAIL;
			}
		}
	}

	sprintf(cmd, "rm -f %s", RSF_IMG);

	system(cmd);

	printf("\n ### hk_dbg, %s : Erase Block-%d OK!!\n", __FUNCTION__, block);

	return result;
}
//------------------------------------------------
