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
#include "mt41kxx.h"

//	Variable
u8  vMode;					//	Verify Mode				

//	Function
u8  fnMT41KxxInit(u8 mode);
u8	fnMT41KxxVerify(u64 start, u64 end);
u8  fnMT41KxxFunRead(u64 start, u32 *data);
u8  fnMT41KxxFunWrite(u64 start, u32 data);

//	MT41KXXX SDRAM  Access Function
SDRAM_FUNC_T mt41kxx =
{
	fnMT41KxxInit,			//	Initial Function
	fnMT41KxxVerify,		//	Self Test
	fnMT41KxxFunRead,		//	Read Function
	fnMT41KxxFunWrite,		//	Write Function
};

/*
 * date: 2016/08/24
 * modify : hk
 * description: Initial MT41KXXX Function
 */
u8	fnMT41KxxInit(u8 mode)
{
	vMode = mode;
	
	return HK_SUCCESS;
}
//------------------------------------------------

/*
 * date: 2016/08/31
 * modify : hk
 * description: Self Test Function
 */
u8	fnMT41KxxVerify(u64 start, u64 end)
{
	u8	result;
	s32 fd;
	u64 len;
	u64 *buf, *dummy;

	if((!start) && (!end))
	{
		start = CONFIG_SYS_MEMTEST_START;

		end = CONFIG_SYS_MEMTEST_END;
	}

	len = (end - start) / sizeof(u64);

	//printf("\n  ### hk_dbg, %s : len:%lu!! \n", __FUNCTION__, len);

	//	Open DDR
	if((fd = open(MT41KXX_DEV, O_RDWR)) < 0)
   	{
   		perror("fnMT41KxxVerify");

      	return HK_FAIL;
   	}

   	//	Memory	Mapping
	buf = mmap(NULL, len, PROT_READ|PROT_WRITE, MAP_SHARED, fd, start);

	if (buf == MAP_FAILED)
	{
		printf("\n  ### hk_dbg, %s : Memory Mapping Fail!!\n", __FUNCTION__);

		close(fd);

		return HK_FAIL;
    }

	//	Memory Mapping
	dummy = mmap(NULL, sizeof(u64), PROT_READ|PROT_WRITE, MAP_SHARED, fd, CONFIG_SYS_MEMTEST_SCRATCH);

	if (dummy == MAP_FAILED)
	{
		printf("\n  ### hk_dbg, %s : Dummy Memory Mapping Fail!!\n", __FUNCTION__);

		//	Deletes the mappings for the buffer address range
	    munmap(buf, len);

		close(fd);

		return HK_FAIL;
    }

	if(vMode == VM_ALTERNATIVE)
		result = fnSDRAMTestAlt(buf, dummy, start, len);	//	Alternative
	else if(vMode == VM_QUICK)
		result = fnSDRAMTestQuick(buf, start, len, 0, 0);	//	Quick
	else
		result = HK_FAIL;

	//	Deletes the mappings for the buffer address range
    munmap(buf, len);

	//	Deletes the mappings for the dummy address range
    munmap(dummy, sizeof(u64));

    close(fd);

	return result;
}
//------------------------------------------------

/*
 * date: 2016/08/24
 * modify : hk
 * description: Read Function
 */
u8 fnMT41KxxFunRead(u64 start, u32 *data)
{
	s32 fd;
	u64 *buf;

	if(!start)
		start = CONFIG_SYS_MEMTEST_START;

	//	Open DDR
	if((fd = open(MT41KXX_DEV, O_RDWR)) < 0)
   	{
   		perror("fnMT41KxxVerify");

      	return HK_FAIL;
   	}

   	//	Memory	Mapping
	buf = mmap(NULL, 1, PROT_READ|PROT_WRITE, MAP_SHARED, fd, start);

	if (buf == MAP_FAILED)
	{
		printf("\n  ### hk_dbg, %s : Memory Mapping Fail!!\n", __FUNCTION__);

		close(fd);

		return HK_FAIL;
    }

	//	Read from memory
    *data = *(v64 *)buf;

    //	Deletes the mappings for the buffer address range
    munmap(buf, 1);

    close(fd);

	return HK_SUCCESS;
}
//------------------------------------------------

/*
 * date: 2016/08/24
 * modify : hk
 * description: Write Function
 */
u8 fnMT41KxxFunWrite(u64 start, u32 data)
{
	s32 fd;
	u64 *buf;

	if(!start)
		start = CONFIG_SYS_MEMTEST_START;

	//	Open DDR
	if((fd = open(MT41KXX_DEV, O_RDWR)) < 0)
   	{
   		perror("fnMT41KxxVerify");

      	return HK_FAIL;
   	}

   	//	Memory	Mapping
	buf = mmap(NULL, 1, PROT_READ|PROT_WRITE, MAP_SHARED, fd, start);

	if (buf == MAP_FAILED)
	{
		printf("\n  ### hk_dbg, %s : Memory Mapping Fail!!\n", __FUNCTION__);

		close(fd);

		return HK_FAIL;
    }

	//	Write to memory
    *(v64 *)buf = data;

    //	Deletes the mappings for the buffer address range
    munmap(buf, 1);

    close(fd);

	return HK_SUCCESS;
}
//------------------------------------------------
