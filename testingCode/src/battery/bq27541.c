#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <strings.h>
#include <unistd.h>
#include <fcntl.h>
#include <time.h>
#include <linux/rtc.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/ioctl.h>
#include <pthread.h>
#include "common.h"
#include "bq27541.h"

//	Function
u8 fnBQ27541Control(void);
u8 fnBQ27541Temperature(u32 *temp);
u8 fnBQ27541ChargeVoltage(u32 *vol);
u8 fnBQ27541Status(char *status);
u8 fnBQ27541AverageCurrent(s32 *cur);
u8 fnBQ27541TimeToEmpty(u32	*time);
u8 fnBQ27541TimeToFull(u32 *time);
u8 fnBQ27541CycleCount(u32 *count);
u8 fnBQ27541Capacity(u32 *cap);
u8 fnBQ27541DevNameLength(u8 *len);
u8 fnBQ27541DevName(u8 len);

//	Battery Function
u8 fnBQ27541Get(BATTERY_QUQEUE_T *battery);
void fnBQ27541RegRead(u8 reg, u8 *data);
void fnBQ27541RegWrite(u8 reg, u8 data);

BATTERY_FUNC_T bq27541 =
{
	fnBQ27541Get,		//	Get GGague
	NULL,
	fnBQ27541RegRead,	//	Read Register
	fnBQ27541RegWrite,	//	Wrire Register
};

/*
 * date: 2016/07/13
 * modify : hk
 * description: BQ27541 Temperature
 */
u8 fnBQ27541Control(void)
{
	u32 ctrl;
	s32	buf[16];

	if(fnBatteryRead(BQ27541_I2C_BUS, BQ27541_I2C_ADDRESS, BQ27541_REG_CNTL, buf, 2) == HK_SUCCESS)
	{
		ctrl = (buf[1] << 8) | buf[0];

		hk_dbg("\n  ### hk_dbg, %s : Control Register:0x%04x!!\n", __FUNCTION__, ctrl);

		return HK_SUCCESS;
	}
	else
	{
		printf("\n  ### hk_dbg, %s : Control Register Read Fail!!\n", __FUNCTION__);

		return HK_FAIL;
	}

}
//-----------------------------------------------------

/*
 * date: 2016/07/11
 * modify : hk
 * description: BQ27541 Temperature
 */
u8 fnBQ27541Temperature(u32 *temp)
{
	s32	buf[16];

	if(fnBatteryRead(BQ27541_I2C_BUS, BQ27541_I2C_ADDRESS, BQ27541_REG_TEMP, buf, 2) == HK_SUCCESS)
	{
		*temp = (buf[1] << 8) | buf[0];

		*temp = *temp / BQ27541_TEMP_UNIT;

		//hk_dbg("\n  ### hk_dbg, %s : Temperature:%d!!\n", __FUNCTION__, temp / BQ27541_TEMP_UNIT);

		return HK_SUCCESS;
	}
	else
	{
		printf("\n  ### hk_dbg, %s : Temperature Fail!!\n", __FUNCTION__);

		return HK_FAIL;
	}	
}
//-----------------------------------------------------

/*
 * date: 2016/07/11
 * modify : hk
 * description: BQ27541 Voltage
 */
u8 fnBQ27541ChargeVoltage(u32 *vol)
{
	s32	buf[16];

	if(fnBatteryRead(BQ27541_I2C_BUS, BQ27541_I2C_ADDRESS, BQ27541_REG_CV, buf, 2) == HK_SUCCESS)
	{
		*vol = (buf[1] << 8) | buf[0];

		*vol = *vol * BQ27541_VOLTAGE_UNIT;

		//hk_dbg("\n  ### hk_dbg, %s : Voltage:%d!!\n", __FUNCTION__, vol * BQ27541_VOLTAGE_UNIT);

		return HK_SUCCESS;
	}
	else
	{
		printf("\n  ### hk_dbg, %s : Voltage Fail!!\n", __FUNCTION__);
	
		return HK_FAIL;
	}
}
//-----------------------------------------------------

/*
 * date: 2016/07/11
 * modify : hk
 * description: BQ27541 Status
 */
u8 fnBQ27541Status(char *status)
{
	u32 flag = 0;
	s32	buf[16];

	if(fnBatteryRead(BQ27541_I2C_BUS, BQ27541_I2C_ADDRESS, BQ27541_REG_FLAGS, buf, 2) == HK_SUCCESS)
	{
		flag = (buf[1] << 8) | buf[0];

		if(flag & BQ27541_FB_FC)
		{
			//hk_dbg("\n  ### hk_dbg, %s : Full-charged!\n", __FUNCTION__);
			strcpy(status, "Full-charged");
		}	
		else
		{
			if(flag & BQ27541_FB_DSG)
			{
				//hk_dbg("\n  ### hk_dbg, %s : Discharging!\n", __FUNCTION__);
				strcpy(status, "Discharging");
			}	
			else
			{
				//hk_dbg("\n  ### hk_dbg, %s : Charging!\n", __FUNCTION__);
				strcpy(status, "Charging");
			}	
		}

		return HK_SUCCESS;
	}
	else
	{
		printf("\n  ### hk_dbg, %s : Status Fail!!\n", __FUNCTION__);

		return HK_FAIL;
	}	
}
//-----------------------------------------------------

/*
 * date: 2016/07/11
 * modify : hk
 * description: BQ27541 Current
 */
u8 fnBQ27541AverageCurrent(s32 *cur)
{
	s32	buf[16];

	if(fnBatteryRead(BQ27541_I2C_BUS, BQ27541_I2C_ADDRESS, BQ27541_REG_AI, buf, 2) == HK_SUCCESS)
	{
		*cur = (buf[1] << 8) | buf[0];

		*cur = *cur * BQ27541_CURRENT_UNIT;

		//hk_dbg("\n  ### hk_dbg, %s : Current:%d!!\n", __FUNCTION__, current * BQ27541_CURRENT_UNIT);

		return HK_SUCCESS;
	}
	else
	{
		printf("\n  ### hk_dbg, %s : Current Fail!!\n", __FUNCTION__);

		return HK_FAIL;
	}	
}
//-----------------------------------------------------

/*
 * date: 2016/07/12
 * modify : hk
 * description: BQ27541 Time to Empty
 */
u8 fnBQ27541TimeToEmpty(u32	*time)
{
	s32	buf[16];

	if(fnBatteryRead(BQ27541_I2C_BUS, BQ27541_I2C_ADDRESS, BQ27541_REG_TTE, buf, 2) == HK_SUCCESS)
	{
		*time = (buf[1] << 8) | buf[0];

		hk_dbg("\n  ### hk_dbg, %s : Time to Empty:%d!!\n", __FUNCTION__, *time);

		return HK_SUCCESS;
	}
	else
	{
		printf("\n  ### hk_dbg, %s : Time to Empty Fail!!\n", __FUNCTION__);

		return HK_FAIL;
	}
}
//-----------------------------------------------------

/*
 * date: 2016/07/12
 * modify : hk
 * description: BQ27541 Time to Full
 */
u8 fnBQ27541TimeToFull(u32 *time)
{
	s32	buf[16];

	if(fnBatteryRead(BQ27541_I2C_BUS, BQ27541_I2C_ADDRESS, BQ27541_REG_TTF, buf, 2) == HK_SUCCESS)
	{
		*time = (buf[1] << 8) | buf[0];

		hk_dbg("\n  ### hk_dbg, %s : Time to Full:%d!!\n", __FUNCTION__, *time);

		return HK_SUCCESS;
	}
	else
	{
		printf("\n  ### hk_dbg, %s : Time to Full Fail!!\n", __FUNCTION__);

		return HK_FAIL;
	}
}
//-----------------------------------------------------

/*
 * date: 2016/07/12
 * modify : hk
 * description: BQ27541 Cycle Count
 */
u8 fnBQ27541CycleCount(u32 *count)
{
	s32	buf[16];

	if(fnBatteryRead(BQ27541_I2C_BUS, BQ27541_I2C_ADDRESS, BQ27541_REG_CC, buf, 2) == HK_SUCCESS)
	{
		*count = (buf[1] << 8) | buf[0];

		hk_dbg("\n  ### hk_dbg, %s : Cycle Count:%d!!\n", __FUNCTION__, *count);

		return HK_SUCCESS;
	}
	else
	{
		printf("\n  ### hk_dbg, %s : Cycle Count Read Fail!!\n", __FUNCTION__);

		return HK_FAIL;
	}
}
//-----------------------------------------------------

/*
 * date: 2016/07/11
 * modify : hk
 * description: BQ27541 State of Charge (%)
 */
u8 fnBQ27541Capacity(u32 *cap)
{
	s32	buf[16];

	if(fnBatteryRead(BQ27541_I2C_BUS, BQ27541_I2C_ADDRESS, BQ27541_REG_SOC, buf, 2) == HK_SUCCESS)
	{
		*cap = (buf[1] << 8) | buf[0];

		//hk_dbg("\n  ### hk_dbg, %s : Capacity:%d!!\n", __FUNCTION__, soc);

		return HK_SUCCESS;
	}
	else
	{
		printf("\n  ### hk_dbg, %s : Capacity Fail!!\n", __FUNCTION__);

		return HK_FAIL;
	}	
}
//-----------------------------------------------------

/*
 * date: 2016/07/12
 * modify : hk
 * description: BQ27541 Device Name Length
 */
u8 fnBQ27541DevNameLength(u8 *len)
{
	s32	buf[16];

	if(fnBatteryRead(BQ27541_I2C_BUS, BQ27541_I2C_ADDRESS, BQ27541_REG_DNAMELEN, buf, 1) == HK_SUCCESS)
	{
		*len = buf[0];

		hk_dbg("\n  ### hk_dbg, %s : Device Name Length:%d!!\n", __FUNCTION__, *len);

		return HK_SUCCESS;
	}
	else
	{
		*len = 0;

		printf("\n  ### hk_dbg, %s : Capacity Fail!!\n", __FUNCTION__);

		return HK_FAIL;
	}
}
//-----------------------------------------------------

/*
 * date: 2016/07/12
 * modify : hk
 * description: BQ27541 Device Name
 */
u8 fnBQ27541DevName(u8 len)
{
	s32	buf[16];

	//	Device Name
	if(fnBatteryRead(BQ27541_I2C_BUS, BQ27541_I2C_ADDRESS, BQ27541_REG_DNAME, buf, len) == HK_SUCCESS)
	{
		u8 i;

		printf("\n  ### hk_dbg, %s : Device Name:", __FUNCTION__);

		for(i=0;i<len;i++)
			printf("%c", buf[i]);

		printf(" !!\n");

		return HK_SUCCESS;
	}
	else
	{
		printf("\n  ### hk_dbg, %s : Device Name Read Fail!!\n", __FUNCTION__);

		return HK_FAIL;
	}
}
//-----------------------------------------------------

/*
 * date: 2016/07/02
 * modify : hk
 * description: Get BQ27541
 */
u8 fnBQ27541Get(BATTERY_QUQEUE_T *battery)
{
	//	State of Charge
	if(fnBQ27541Capacity(&battery->cap) == HK_FAIL)
		return HK_FAIL;

	//	Temperature
	if(fnBQ27541Temperature(&battery->temp) == HK_FAIL)
		return HK_FAIL;

	//	Current
	if(fnBQ27541AverageCurrent(&battery->cur) == HK_FAIL)
		return HK_FAIL;

	//	Voltage
	if(fnBQ27541ChargeVoltage(&battery->vol) == HK_FAIL)
		return HK_FAIL;

	//	Status
	if(fnBQ27541Status(battery->status) == HK_FAIL)
		return HK_FAIL;

	return HK_SUCCESS;
}
//------------------------------------------------

/*
 * date: 2016/07/12
 * modify : hk
 * description: Read BQ27541 Register
 */
void fnBQ27541RegRead(u8 reg, u8 *data)
{
	s32	buf[16];

	if(fnBatteryRead(BQ27541_I2C_BUS, BQ27541_I2C_ADDRESS, reg, buf, 1) == HK_SUCCESS)
		*data = buf[0];
	else
		*data = 0xFF;

	return;
}
//------------------------------------------------

/*
 * date: 2016/07/12
 * modify : hk
 * description: Write BQ27541 Register
 */
void fnBQ27541RegWrite(u8 reg, u8 data)
{
	u8	buf[16];

	buf[0] = data;

	if(fnBatteryWrite(BQ27541_I2C_BUS, BQ27541_I2C_ADDRESS, reg, buf, 1) == HK_SUCCESS)
		printf("\n  ### hk_dbg, %s : Write BQ27541 Register(0x%02x) Success!! \n", __FUNCTION__, reg);
	else
		printf("\n  ### hk_dbg, %s : Write BQ27541 Register(0x%02x) Fail!! \n", __FUNCTION__, reg);
	return;
}
//------------------------------------------------
