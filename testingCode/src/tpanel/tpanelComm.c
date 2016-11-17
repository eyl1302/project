#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <strings.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/ioctl.h>
#include <fcntl.h>
#include "i2c-dev.h"
#include "common.h"
#include "system.h"
#include "tpanel.h"
#include "tpanelComm.h"

/*
 * date: 2016/10/14
 * modify : hk
 * description: Touch Panel Read Function
 */
u8 fnTPanelRead(char* bus, u8 addr, u8 reg, u8 *buf, u8 len)
{
	s32 fd;

	//	Open Touch Panel I2C Bus
	if((fd = open(bus, O_RDWR)) < 0)
   	{
   		hk_dbg("\n  ### hk_dbg: %s, line:%d, Failed to open Touch Panel bus!!  ###\n ", __FUNCTION__, __LINE__);

      	return HK_FAIL;
   	}

	if (ioctl(fd, I2C_SLAVE_FORCE, addr) < 0)
	{
		hk_dbg("\n  ### hk_dbg: %s, line:%d, Could not set address to 0x%02x:!!  ###\n ", __FUNCTION__, __LINE__, addr);

   		//	Close Touch Panel
		close(fd);

		return HK_FAIL;;
	}

	if(i2c_smbus_read_i2c_block_data(fd, reg, len, buf) < 0)
	{
		hk_dbg("\n  ### hk_dbg: %s, line:%d, Unable to read from Touch Panel!!  ###\n ", __FUNCTION__, __LINE__);

   		//	Close Touch Panel
		close(fd);

		return HK_FAIL;
	}

   	//	Close Touch Panel 
	close(fd);

	return HK_SUCCESS;
}
//------------------------------------------------

/*
 * date: 2016/10/14
 * modify : hk
 * description: Touch Panel Write Function
 */
u8 fnTPanelWrite(char* bus, u8 addr, u8 reg, u8 *buf, u8 len)
{
	s32 fd;

	//	Open Touch Panel I2C Bus
	if((fd = open(bus, O_RDWR)) < 0)
   	{
      	hk_dbg("\n  ### hk_dbg: %s, line:%d, Failed to open Touch Panel bus!!  ###\n ", __FUNCTION__, __LINE__);

      	return HK_FAIL;
   	}

	//	I2C Slave Address
   	if (ioctl(fd, I2C_SLAVE_FORCE, addr) < 0)
	{
		hk_dbg("\n  ### hk_dbg: %s, line:%d, Could not set address to 0x%02x:!!  ###\n ", __FUNCTION__, __LINE__, addr);

   		//	Close Touch Panel
		close(fd);

		return HK_FAIL;;
	}

	if(i2c_smbus_write_i2c_block_data(fd, reg, len, buf) < 0)
	{
   		hk_dbg("\n  ### hk_dbg: %s, line:%d, Unable to write to Touch Panel!!  ###\n ", __FUNCTION__, __LINE__);

   		//	Close Touch Panel
		close(fd);

   		return HK_FAIL;
   	}

   	//	Close Touch Panel
	close(fd);

	return HK_SUCCESS;
}
//------------------------------------------------
