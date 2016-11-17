#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <strings.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/ioctl.h>
#include <fcntl.h>
#include "i2c-dev.h"
#include "common.h"
#include "gsensorComm.h"

/*
 * date: 2016/07/18
 * modify : hk
 * description: GSensor Read Function
 */
u8 fnGSensorRead(char* bus, u8 addr, u8 reg, u8 *buf, u8 len)
{
	s32 fd;

	//	Open RTC I2C Bus
	if((fd = open(bus, O_RDWR)) < 0)
   	{
   		perror("fnRTCRead");

      	return HK_FAIL;
   	}

	if (ioctl(fd, I2C_SLAVE_FORCE, addr) < 0)
	{
		hk_dbg("\n  ### hk_dbg: %s, line:%d, Could not set address to 0x%02x:!!  ###\n ", __FUNCTION__, __LINE__, addr);

   		//	Close RTC
		close(fd);

		return HK_FAIL;;
	}

	if(i2c_smbus_read_i2c_block_data(fd, reg, len, buf) < 0)
	{
		hk_dbg("\n  ### hk_dbg: %s, line:%d, Unable to read from RTC!!  ###\n ", __FUNCTION__, __LINE__);

   		//	Close RTC
		close(fd);

		return HK_FAIL;
	}

   	//	Close RTC
	close(fd);

	return HK_SUCCESS;
}
//-----------------------------------------------------

/*
 * date: 2016/07/18
 * modify : hk
 * description: Gsensor Write Function
 */
u8 fnGSensorWrite(char* bus, u8 addr, u8 reg, u8 *buf, u8 len)
{
	s32 fd;

	//	Open RTC I2C Bus
	if((fd = open(bus, O_RDWR)) < 0)
   	{
      	hk_dbg("\n  ### hk_dbg: %s, line:%d, Failed to open i2c bus!!  ###\n ", __FUNCTION__, __LINE__);

      	return HK_FAIL;
   	}

	//	I2C Slave Address
   	if (ioctl(fd, I2C_SLAVE_FORCE, addr) < 0)
	{
		hk_dbg("\n  ### hk_dbg: %s, line:%d, Could not set address to 0x%02x:!!  ###\n ", __FUNCTION__, __LINE__, addr);

   		//	Close RTC
		close(fd);

		return HK_FAIL;;
	}

	if(i2c_smbus_write_i2c_block_data(fd, reg, len, buf) < 0)
	{
   		hk_dbg("\n  ### hk_dbg: %s, line:%d, Unable to write to RTC!!  ###\n ", __FUNCTION__, __LINE__);

   		//	Close RTC
		close(fd);

   		return HK_FAIL;
   	}

   	//	Close RTC
	close(fd);

	return HK_SUCCESS;
}
//-----------------------------------------------------
