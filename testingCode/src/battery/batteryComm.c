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
#include "batteryComm.h"

/*
 * date: 2016/07/11
 * modify : hk
 * description: Battery Read Function
 */
u8 fnBatteryRead(char* bus, u8 addr, u8 reg, s32 *buf, u8 len)
{
	u8  i;
	s32 fd;

	//	Open Gsensor I2C Bus
	if((fd = open(bus, O_RDWR)) < 0)
   	{
   		perror("fnGGuageRead");
     
      	return HK_FAIL;
   	}

	if (ioctl(fd, I2C_SLAVE_FORCE, addr) < 0)
	{
		printf("\n ### hk_dbg: %s, line:%d, Could not set address to 0x%02x:!!  ###\n ", __FUNCTION__, __LINE__, addr);

   		//	Cose Gas Guage
		close(fd);
		
		return HK_FAIL;;
	}

   	for(i=0;i<len;i++)
   	{
		buf[i] = i2c_smbus_read_byte_data(fd, (reg+i));

   		if(buf[i]  < 0)
   		{
   			printf("\n ### hk_dbg: %s, line:%d, Unable to read from Battery!!  ###\n ", __FUNCTION__, __LINE__);

   			//	Cose Gas Guage
			close(fd);

   			return HK_FAIL;
   		}
   	}

   	//	Cose Gas Guage
	close(fd);

	return HK_SUCCESS;
}
//-----------------------------------------------------

/*
 * date: 2016/07/11
 * modify : hk
 * description: Battery Write Function
 */
u8 fnBatteryWrite(char* bus, u8 addr, u8 reg, u8 *buf, u8 len)
{
	s32 fd;

	//	Open Gsensor I2C Bus
	if((fd = open(bus, O_RDWR)) < 0)
   	{
      	printf("\n ### hk_dbg: %s, line:%d, Failed to open i2c bus!!  ###\n ", __FUNCTION__, __LINE__);
    
      	return HK_FAIL;
   	}
    
   	//	I2C Slave Address
	if(ioctl(fd, I2C_SLAVE_FORCE, addr) < 0)
   	{
   		printf("\n ### hk_dbg: %s, line:%d, Could not set address to 0x%02x:!!  ###\n ", __FUNCTION__, __LINE__, addr);

   		//	Cose Gas Guage
		close(fd);
    
   		return HK_FAIL;
   	}

   	if(i2c_smbus_write_i2c_block_data(fd, reg, len, buf) < 0)
	{
   		printf("\n ### hk_dbg: %s, line:%d, Unable to write to Battery!!  ###\n ", __FUNCTION__, __LINE__);

   		//	Cose Gas Guage
		close(fd);

   		return HK_FAIL;
   	}

   	//	Cose RTC
	close(fd);

	return HK_SUCCESS;
}
//-----------------------------------------------------
