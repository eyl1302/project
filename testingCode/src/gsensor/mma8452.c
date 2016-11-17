#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <strings.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include "i2c-dev.h"
#include "common.h"
#include "system.h"
#include "mma8452.h"

//	Important : Registers can be changed in Standby Mode.

//	Internal Function
u8    fnMMA8452GetFastReadMode(u8 *mode);
u8    fnMMA8452GetFullScanRange(u8 *fs);
s16   fnMMA8452CalculateComplement(u16 data, u8 mode, u8 fs, u16 max);
u8	  fnMMA8452CalibrationAuto(void);
u8	  fnMMA8452CalibrationAccurate(void);
s16   fnMMA8452GetComplement(u16 data);
float fnMMA8452GetGravity(u16 data);
void  fnMMA8452DetectMotion(void);
void  fnMMA8452DetectFreefall(void);

//	Function Pointer
u8 fnMMA8452Init(u8 mode);
u8 fnMMA8452ReadOut(GSENSOR_OUTPUT *gsr);
u8 fnMMA8452Calibration(u8 mode);		//	0: Auto, 1:Accurate
u8 fnMMA8452RegRead(u8 reg, u8 *data);
u8 fnMMA8452RegWrite(u8 reg, u8 data);

GSENSOR_FUNC_T mma8452 =
{
	fnMMA8452Init,
	fnMMA8452ReadOut,
	fnMMA8452Calibration,
	fnMMA8452RegRead,
	fnMMA8452RegWrite,
};

/*
 * date: 2016/07/19
 * modify : hk
 * description: Initial MMA8452
 */
u8 fnMMA8452Init(u8 mode)
{
	//	Motion / Freefall Mode
	if(!mode)
		fnMMA8452DetectMotion();
	else
		fnMMA8452DetectFreefall();

	return HK_SUCCESS;
}
//------------------------------------------------

/*
 * date: 2016/07/19
 * modify : hk
 * description: Read MMA8452 X,Y,Z Out
 */
u8 fnMMA8452ReadOut(GSENSOR_OUTPUT *gsr)
{
	u8 	buf[32];
	u16 data;

	//	Get Device ID
	if(fnGSensorRead(MMA8452_I2C_BUS, MMA8452_I2C_ADDRESS, REG_WHO_AM_I, buf, 1) == HK_FAIL)
		return HK_FAIL;

	if(buf[0] != MMA8452_DEV_ID)	//	Device ID Check
		return HK_FAIL;

	//// 	Get Overwrite Status
	//if(fnGSensorRead(MMA8452_I2C_BUS, MMA8452_I2C_ADDRESS, REG_STATUS, buf, 1) == HK_FAIL)
	//	return HK_FAIL;
    //
	//if(buf[0] == 0x00)				//	No overwrite occured
	//	return HK_FAIL;

	//	X Out
	if(fnGSensorRead(MMA8452_I2C_BUS, MMA8452_I2C_ADDRESS, REG_OUT_X_MSB, buf, 2) == HK_FAIL)
		return HK_FAIL;

	//printf("\n  ### hk_dbg, %s : 0x%02x, 0x%02x!\n", __FUNCTION__, buf[0], buf[1]);

	data = GOUT_CALCULATE(buf[0], buf[1]);

	gsr->x = fnMMA8452GetGravity(data);

	//	Y Out
	if(fnGSensorRead(MMA8452_I2C_BUS, MMA8452_I2C_ADDRESS, REG_OUT_Y_MSB, buf, 2) == HK_FAIL)
		return HK_FAIL;

	//printf("\n  ### hk_dbg, %s : 0x%02x, 0x%02x!\n", __FUNCTION__, buf[0], buf[1]);

	data = GOUT_CALCULATE(buf[0], buf[1]);

	gsr->y = fnMMA8452GetGravity(data);

	//	Z Out
	if(fnGSensorRead(MMA8452_I2C_BUS, MMA8452_I2C_ADDRESS, REG_OUT_Z_MSB, buf, 2) == HK_FAIL)
		return HK_FAIL;

	//printf("\n  ### hk_dbg, %s : 0x%02x, 0x%02x!\n", __FUNCTION__, buf[0], buf[1]);

	data = GOUT_CALCULATE(buf[0], buf[1]);

	gsr->z = fnMMA8452GetGravity(data);

	return HK_SUCCESS;
}
//------------------------------------------------

/*
 * date: 2016/07/22
 * modify : hk
 * description: Read MMA8452
 */
u8 fnMMA8452Calibration(u8 mode)
{
	u8 value;

	if(!mode)
		value = fnMMA8452CalibrationAuto();		//	Auto Calibration
	else
		value = fnMMA8452CalibrationAccurate();	//	Accurate Calibration

	return value;
}
//------------------------------------------------

/*
 * date: 2016/07/18
 * modify : hk
 * description: Read MMA8452
 */
u8 fnMMA8452RegRead(u8 reg, u8 *data)
{
	u8 	buf[32];

	//	Read GSensor
	if(fnGSensorRead(MMA8452_I2C_BUS, MMA8452_I2C_ADDRESS, reg, buf, 1) == HK_SUCCESS)
	{
		*data = buf[0];

		return HK_SUCCESS;
	}
	else
	{
		*data = 0xFF;

		return HK_FAIL;
	}
}
//------------------------------------------------

/*
 * date: 2016/07/18
 * modify : hk
 * description: Write MMA8452
 */
u8 fnMMA8452RegWrite(u8 reg, u8 data)
{
	u8 	buf[32];

	buf[0] = data;

	//	Write GSensor
	if(fnGSensorWrite(MMA8452_I2C_BUS, MMA8452_I2C_ADDRESS, reg, buf, 1) == HK_SUCCESS)
		return HK_SUCCESS;
	else
		return HK_FAIL;
}
//------------------------------------------------

/*
 * date: 2016/07/20
 * modify : hk
 * description: Get Fast Read Mode
 */
u8 fnMMA8452GetFastReadMode(u8 *mode)
{
	u8 buf[32];

	if(fnGSensorRead(MMA8452_I2C_BUS, MMA8452_I2C_ADDRESS, REG_CTRL_1, buf, 1) == HK_SUCCESS)
	{
		*mode = buf[0] & FS_READ;

		//printf("\n  ### hk_dbg, %s : fast read mode:%d\n", __FUNCTION__, *mode);

		return HK_SUCCESS;
	}

	return HK_FAIL;
}
//------------------------------------------------

/*
 * date: 2016/07/20
 * modify : hk
 * description: Get Full Scale Range
 */
u8 fnMMA8452GetFullScanRange(u8 *fs)
{
	u8 data;
	u8 buf[32];

	if(fnGSensorRead(MMA8452_I2C_BUS, MMA8452_I2C_ADDRESS, REG_XYZ_DATA_CFG, buf, 1) == HK_SUCCESS)
	{
		data = buf[0] & FS_RANGE;

		*fs = pow(2,(1+data));

		//printf("\n  ### hk_dbg, %s : full scale range:%d!\n", __FUNCTION__, *fs);

		return HK_SUCCESS;
	}

	return HK_FAIL;
}
//------------------------------------------------

/*
 * date: 2016/07/20
 * modify : hk
 * description: 2's Complement Convert
 */
s16 fnMMA8452CalculateComplement(u16 data, u8 mode, u8 fs, u16 max)
{
	s16 value;

	if(!mode)	//	Normal Mode
	{
		//	2 Complement Convert
		if(data & NORMAL_COMPLEMENT_CHECK)
			value = (data & (~NORMAL_COMPLEMENT_CHECK)) - max;
		else
			value = data;
	}
	else		//	Fast Read Mode
	{
		//	2 Complement Convert
		if(data & FASTREAD_COMPLEMENT_CHECK)
			value = (data & (~FASTREAD_COMPLEMENT_CHECK)) - max;
		else
			value = data;
	}

	//printf("\n  ### hk_dbg, %s : data:%d\n", __FUNCTION__, value);

	return value;
}
//------------------------------------------------

/*
 * date: 2016/07/22
 * modify : hk
 * description: Auto Calibration
 */
u8 fnMMA8452CalibrationAuto(void)
{
	u16 offsetX;
	u16 offsetY;
	u16 offsetZ;

	u8 rBuf[8];
	u8 wBuf[8]={0x00, 0x11, 0x10};

	//	Set Full Scale Range in 2g Mode
	fnGSensorWrite(MMA8452_I2C_BUS, MMA8452_I2C_ADDRESS, REG_XYZ_DATA_CFG, &wBuf[0], 1);

	//	Set Device in 200Hz ODR, Active Mode
	fnGSensorWrite(MMA8452_I2C_BUS, MMA8452_I2C_ADDRESS, REG_CTRL_1, &wBuf[1], 1);

	//	Get X Offset
	if(fnGSensorRead(MMA8452_I2C_BUS, MMA8452_I2C_ADDRESS, REG_OUT_X_MSB, rBuf, 2) == HK_FAIL)
		return HK_FAIL;

	{
		u16 data;
		s16 offset;

		//	Calculate 2's Complement for X
		data = GOUT_CALCULATE(rBuf[0], rBuf[1]);;

		offset = fnMMA8452GetComplement(data);

		//printf("\n  ### hk_dbg, %s : 2's Complement of X:%d \n", __FUNCTION__, offset);

		offset = (offset >> 3);

		if(offset < 0)
			offsetX = offset * (-1);
		else
			offsetX = offset;

		//printf("\n  ### hk_dbg, %s : offset X:%d \n", __FUNCTION__, offsetX);
	}

	//	Get Y Offset
	if(fnGSensorRead(MMA8452_I2C_BUS, MMA8452_I2C_ADDRESS, REG_OUT_Y_MSB, rBuf, 2) == HK_FAIL)
		return HK_FAIL;

	{
		u16 data;
		s16 offset;

		data = GOUT_CALCULATE(rBuf[0], rBuf[1]);

		offset = fnMMA8452GetComplement(data);

		//printf("\n  ### hk_dbg, %s : 2's Complement of Y:%d \n", __FUNCTION__, offset);

		offset = (offset >> 3);

		if(offset < 0)
			offsetY = offset * (-1);
		else
			offsetY = offset;

		//printf("\n  ### hk_dbg, %s : offset Y:%d \n", __FUNCTION__, offsetY);
	}

	//	Get Z Offset
	if(fnGSensorRead(MMA8452_I2C_BUS, MMA8452_I2C_ADDRESS, REG_OUT_Z_MSB, rBuf, 2) == HK_FAIL)
		return HK_FAIL;

	{
		u16 data;
		s16 offset;

		data = GOUT_CALCULATE(rBuf[0], rBuf[1]);

		offset = fnMMA8452GetComplement(data);

		//printf("\n  ### hk_dbg, %s : 2's Complement of Z:%d \n", __FUNCTION__, offset);

		offset = (4096 - offset) >> 3;

		if(offset < 0)
			offsetZ = offset * (-1);
		else
			offsetZ = offset;

		//printf("\n  ### hk_dbg, %s : offset Z:%d \n", __FUNCTION__, offsetZ);
	}

	//	Put device in Standby mode
	fnGSensorWrite(MMA8452_I2C_BUS, MMA8452_I2C_ADDRESS, REG_CTRL_1, &wBuf[2], 1);

	//	Set X, Y, Z OffSet
	wBuf[0] = offsetX;
	wBuf[1] = offsetY;
	wBuf[2] = offsetZ;

	fnGSensorWrite(MMA8452_I2C_BUS, MMA8452_I2C_ADDRESS, REG_OFF_X, wBuf, 3);

	//	Set Device in 200Hz ODR, Active Mode
	fnGSensorWrite(MMA8452_I2C_BUS, MMA8452_I2C_ADDRESS, REG_CTRL_1, &wBuf[1], 1);

	return HK_SUCCESS;
}
//------------------------------------------------

/*
 * date: 2016/07/22
 * modify : hk
 * description: Accurate Calibration
 */
u8 fnMMA8452CalibrationAccurate(void)
{
	MMA8452Q_T x,y,z;
	s16 xOffset, yOffset, zOffset;
	u8 wBuf[8]={0x00, 0x11, 0x10};

	{
		u8  buf[32];

		//	Wait for Enter Key
		fgets((char*)buf, sizeof(buf), stdin);
	}

	//	Set Full Scale Range in 2g Mode
	fnGSensorWrite(MMA8452_I2C_BUS, MMA8452_I2C_ADDRESS, REG_XYZ_DATA_CFG, &wBuf[0], 1);

	//	Set Device in 200Hz ODR, Active Mode
	fnGSensorWrite(MMA8452_I2C_BUS, MMA8452_I2C_ADDRESS, REG_CTRL_1, &wBuf[1], 1);

	printf("\n  There are 6 faces of offset value shoud be read, the sequence: up -> left -> down -> right -> top -> bottom,");
	printf("\n  Please follow the notice and put the diver on the right dircetion!!\n");

	//	Portrait Up
	{
		u8  buf[32];
		u16 data;

		printf("\n  1. Up : 		PU		   ");
		printf("\n          |    ¢z-------¢{     ");
		printf("\n          |    |   o   |     ");
		printf("\n          | 1g |       |     ");
		printf("\n          |    |       |     ");
		printf("\n          |    ¢|-------¢}     "); 
		printf("\n          ¡õ                  ");

		printf("\n  If the device was put on the righ direction, please press enter to read!");

		//	Wait for Enter Key
		fgets((char*)buf, sizeof(buf), stdin);

		//	Read PUx
		if(fnGSensorRead(MMA8452_I2C_BUS, MMA8452_I2C_ADDRESS, REG_OUT_X_MSB, buf, 2) == HK_FAIL)
			return HK_FAIL;

		data = GOUT_CALCULATE(buf[0], buf[1]);

		x.pu = fnMMA8452GetComplement(data);

		//	Read PUz
		if(fnGSensorRead(MMA8452_I2C_BUS, MMA8452_I2C_ADDRESS, REG_OUT_Z_MSB, buf, 2) == HK_FAIL)
			return HK_FAIL;

		data = GOUT_CALCULATE(buf[0], buf[1]);

		z.pu = fnMMA8452GetComplement(data);

		//printf("\n  ### hk_dbg, %s,  x.pu:%d, z.pu::%d \n", __FUNCTION__, x.pu, z.pu);
	}

	//	Landscape Left
	{
		u8  buf[32];
		u16 data;

		printf("\n  2. Left : 		 LL		    ");
		printf("\n          |    ¢z--------¢{     ");
		printf("\n          |    |        |     ");
		printf("\n          | 1g | o      |     ");
		printf("\n          |    |        |     "); 
		printf("\n          |    ¢|--------¢}     ");
		printf("\n          ¡õ                   ");

		printf("\n  If the device was put on the righ direction, please press enter to read!");

		//	Wait for Enter Key
		fgets((char*)buf, sizeof(buf), stdin);

		//	Read LLy
		if(fnGSensorRead(MMA8452_I2C_BUS, MMA8452_I2C_ADDRESS, REG_OUT_Y_MSB, buf, 2) == HK_FAIL)
			return HK_FAIL;

		data = GOUT_CALCULATE(buf[0], buf[1]);

		y.ll = fnMMA8452GetComplement(data);

		//	Read LLz
		if(fnGSensorRead(MMA8452_I2C_BUS, MMA8452_I2C_ADDRESS, REG_OUT_Z_MSB, buf, 2) == HK_FAIL)
			return HK_FAIL;

		data = GOUT_CALCULATE(buf[0], buf[1]);

		z.ll = fnMMA8452GetComplement(data);

		//printf("\n  ### hk_dbg, %s,  y.ll:%d, z.ll::%d \n", __FUNCTION__, y.ll, z.ll);
	}

	//	Portrait Down
	{
		u8  buf[32];
		u16 data;

		printf("\n  3. Down : 		 PD			");
		printf("\n          |    ¢z-------¢{      ");
		printf("\n          |    |       |      ");
		printf("\n          | 1g |       |      ");
		printf("\n          |    |   o   |      ");
		printf("\n          |    ¢|-------¢}      ");
		printf("\n          ¡õ                   ");

		printf("\n  If the device was put on the righ direction, please press enter to read!");

		//	Wait for Enter Key
		fgets((char*)buf, sizeof(buf), stdin);

		//	Read PDx
		if(fnGSensorRead(MMA8452_I2C_BUS, MMA8452_I2C_ADDRESS, REG_OUT_X_MSB, buf, 2) == HK_FAIL)
			return HK_FAIL;

		data = GOUT_CALCULATE(buf[0], buf[1]);

		x.pd = fnMMA8452GetComplement(data);

		//	Read PDz
		if(fnGSensorRead(MMA8452_I2C_BUS, MMA8452_I2C_ADDRESS, REG_OUT_Z_MSB, buf, 2) == HK_FAIL)
			return HK_FAIL;

		data = GOUT_CALCULATE(buf[0], buf[1]);

		z.pd = fnMMA8452GetComplement(data);

		//printf("\n  ### hk_dbg, %s,  x.pd:%d, z.pd:%d \n", __FUNCTION__, x.pd, z.pd);
	}

	//	Landscape Right
	{
		u8  buf[32];
		u16 data;

		printf("\n  4. Right : 		 LR			");
		printf("\n          |    ¢z--------¢{     ");
		printf("\n          |    |        |     ");
		printf("\n          | 1g |      o |     "); 
		printf("\n          |    |        |     ");
		printf("\n          |    ¢|--------¢}     ");
		printf("\n          ¡õ                   ");

		printf("\n  If the device was put on the righ direction, please press enter to read!");

		//	Wait for Enter Key
		fgets((char*)buf, sizeof(buf), stdin);

		//	Read LRy
		if(fnGSensorRead(MMA8452_I2C_BUS, MMA8452_I2C_ADDRESS, REG_OUT_Y_MSB, buf, 2) == HK_FAIL)
			return HK_FAIL;

		data = GOUT_CALCULATE(buf[0], buf[1]);

		y.lr = fnMMA8452GetComplement(data);

		//	Read LRz
		if(fnGSensorRead(MMA8452_I2C_BUS, MMA8452_I2C_ADDRESS, REG_OUT_Z_MSB, buf, 2) == HK_FAIL)
			return HK_FAIL;

		data = GOUT_CALCULATE(buf[0], buf[1]);

		z.lr = fnMMA8452GetComplement(data);

		//printf("\n  ### hk_dbg, %s,  y.lr:%d, z.lr:%d \n", __FUNCTION__, y.lr, z.lr);
	}

	//	Front
	{
		u8  buf[32];
		u16 data;

		printf("\n  5. Front :           FRONT               ");
		printf("\n                    |--------|             ");
		printf("\n             -----------------------       ");
		printf("\n");

		printf("\n  If the device was put on the righ direction, please press enter to read!");

		//	Wait for Enter Key
		fgets((char*)buf, sizeof(buf), stdin);

		//	Read FRONTx
		if(fnGSensorRead(MMA8452_I2C_BUS, MMA8452_I2C_ADDRESS, REG_OUT_X_MSB, buf, 2) == HK_FAIL)
			return HK_FAIL;

		data = GOUT_CALCULATE(buf[0], buf[1]);

		x.front = fnMMA8452GetComplement(data);

		//	Read FRONTy
		if(fnGSensorRead(MMA8452_I2C_BUS, MMA8452_I2C_ADDRESS, REG_OUT_Y_MSB, buf, 2) == HK_FAIL)
			return HK_FAIL;

		data = GOUT_CALCULATE(buf[0], buf[1]);

		y.front = fnMMA8452GetComplement(data);

		//printf("\n  ### hk_dbg, %s,  x.front:%d, y.front:%d \n", __FUNCTION__, x.front, y.front);
	}


	//	Back
	{
		u8  buf[32];
		u16 data;

		printf("\n  6. Back :            BACK                ");
		printf("\n              -----------------------      ");
		printf("\n                    |--------|             ");
		printf("\n");

		printf("\n  If the device was put on the righ direction, please press enter to read!");

		//	Wait for Enter Key
		fgets((char*)buf, sizeof(buf), stdin);

		//	Read BACKx
		if(fnGSensorRead(MMA8452_I2C_BUS, MMA8452_I2C_ADDRESS, REG_OUT_X_MSB, buf, 2) == HK_FAIL)
			return HK_FAIL;

		data = GOUT_CALCULATE(buf[0], buf[1]);

		x.back = fnMMA8452GetComplement(data);

		//	Read BACKy
		if(fnGSensorRead(MMA8452_I2C_BUS, MMA8452_I2C_ADDRESS, REG_OUT_Y_MSB, buf, 2) == HK_FAIL)
			return HK_FAIL;

		data = GOUT_CALCULATE(buf[0], buf[1]);

		y.back = fnMMA8452GetComplement(data);

		//printf("\n  ### hk_dbg, %s,  x.back:%d, y.back:%d \n", __FUNCTION__, x.back, y.back);
	}

	xOffset = (x.pu + x.pd + x.front + x.back) >> 2;
	yOffset = (y.ll + y.lr + y.front + y.back) >> 2;
	zOffset = (z.pu + z.ll + z.pd + z.lr) >> 2;

	printf("\n  ### hk_dbg, %s : x:%d, y:%d, z:%d!!\n", __FUNCTION__, xOffset, yOffset, zOffset);

	if(1)
	{
		u8 ctrl;
		u8 buf[32];

		//	Read Control-1
		if(fnGSensorRead(MMA8452_I2C_BUS, MMA8452_I2C_ADDRESS, REG_CTRL_1, buf, 1) == HK_FAIL)
			return HK_FAIL;

		buf[0] = buf[0] & (~0x01);

		ctrl = buf[0];

		//	Put device in Standby mode
		fnGSensorWrite(MMA8452_I2C_BUS, MMA8452_I2C_ADDRESS, REG_CTRL_1, buf, 1);

		buf[0] = xOffset;
		buf[1] = yOffset;
		buf[2] = zOffset;

		//	Set Offset for Cailbration
		fnGSensorWrite(MMA8452_I2C_BUS, MMA8452_I2C_ADDRESS, REG_OFF_X, buf, 3);

		buf[0] = ctrl | 0x01 ;

		//	Put device in Active mode
		fnGSensorWrite(MMA8452_I2C_BUS, MMA8452_I2C_ADDRESS, REG_CTRL_1, buf, 1);
	}


	return HK_SUCCESS;
}
//------------------------------------------------

/*
 * date: 2016/07/21
 * modify : hk
 * description: Geat Gravity
 */
s16  fnMMA8452GetComplement(u16 data)
{
	u8		fs;		//	full scale
	u8  	mode;	//	0: normal(12-bit), 1:fast read(8-bit)
	u16		max;
	s16   	value;

	//	Get Fast Read Mode
	if(fnMMA8452GetFastReadMode(&mode) == HK_FAIL)
		return 0;

	//	Calculate Divider
	max = pow(2, FS_READ_MODE(mode));

	//	Get Full Scanle
	if(fnMMA8452GetFullScanRange(&fs) == HK_FAIL)
		return 0;

	//	2's Complement Convert
	value = fnMMA8452CalculateComplement(data, mode, fs, max);

	return value;
}
//------------------------------------------------

/*
 * date: 2016/07/19
 * modify : hk
 * description: Geat Gravity
 */
float fnMMA8452GetGravity(u16 data)
{
	u8		fs;		//	full scale
	u8  	mode;	//	0: normal(12-bit), 1:fast read(8-bit)
	u16		max;
	s16   	value;
	float 	gValue;

	//	Get Fast Read Mode
	if(fnMMA8452GetFastReadMode(&mode) == HK_FAIL)
		return 0;

	//	Calculate Divider
	max = pow(2, (FS_READ_MODE(mode)-1));

	//printf("### hk_dbg, %s : max:%d\n", __FUNCTION__, max);

	//	Get Full Scanle
	if(fnMMA8452GetFullScanRange(&fs) == HK_FAIL)
		return 0;

	//	2's Complement Convert
	value = fnMMA8452CalculateComplement(data, mode, fs, max);

	//	Gravity Convert
	gValue = (float)(value) * (float)fs / (float)max ;

	return gValue;
}
//------------------------------------------------

/*
 * date: 2016/07/19
 * modify : hk
 * description: Motion Detect Initial
 */
void fnMMA8452DetectMotion(void)
{
	u8 buf[8] = {0x18, 0xd8, 0x20, 0x0a, 0x04, 0x04, 0x19};

	//	Set Device in 100Hz ODR, Standby Mode
	fnGSensorWrite(MMA8452_I2C_BUS, MMA8452_I2C_ADDRESS, REG_CTRL_1, &buf[0], 1);

	//	Set Configuration Register for Motion Detection by setting the "OR" condition OAE = 1, enabling X, Y, and the latch
	fnGSensorWrite(MMA8452_I2C_BUS, MMA8452_I2C_ADDRESS, REG_MT_CFG, &buf[1], 1);

	//	Threshold Setting Value for the Motion detection of > 3g,
	//	Note: The step count is 0.063g/ count
	//	3g/0.063g = 47.6; //Round up to 48
	fnGSensorWrite(MMA8452_I2C_BUS, MMA8452_I2C_ADDRESS, REG_MT_THS, &buf[2], 1);

	//	Set the debounce counter to eliminate false readings for 100 Hz sample rate with a requirement of 100 ms timer.
	//	Note: 100 ms/10 ms (steps) = 10 counts
	fnGSensorWrite(MMA8452_I2C_BUS, MMA8452_I2C_ADDRESS, REG_MT_COUNT, &buf[3], 1);

	//	Enable Motion/Freefall Interrupt Function in the System
	fnGSensorWrite(MMA8452_I2C_BUS, MMA8452_I2C_ADDRESS, REG_CTRL_4, &buf[4], 1);

	//	Route the Motion/Freefall Interrupt Function to INT1 hardware pin
	fnGSensorWrite(MMA8452_I2C_BUS, MMA8452_I2C_ADDRESS, REG_CTRL_5, &buf[5], 1);

	//	Put the device in Active Mode
	fnGSensorWrite(MMA8452_I2C_BUS, MMA8452_I2C_ADDRESS, REG_CTRL_1, &buf[6], 1);

	return;
}
//------------------------------------------------

/*
 * date: 2016/07/19
 * modify : hk
 * description: Free Fall Detect Initial
 */
void fnMMA8452DetectFreefall(void)
{
	u8 buf[8] = {0x20, 0xb8, 0x03, 0x06, 0x04, 0x00, 0x21};

	//	Set Device in 50Hz ODR, Standby Mode
	fnGSensorWrite(MMA8452_I2C_BUS, MMA8452_I2C_ADDRESS, REG_CTRL_1, &buf[0], 1);

	//	Configuration Register set for Freefall Detection enabling \A1\A7AND\A1\A8 condition, OAE = 0, Enabling X, Y, Z and the Latch
	fnGSensorWrite(MMA8452_I2C_BUS, MMA8452_I2C_ADDRESS, REG_MT_CFG, &buf[1], 1);

	//	Threshold Setting Value for the resulting acceleration < 0.2g
	//	Note: The step count is 0.063g / count
	//	0.2g/0.063g = 3.17 counts //Round to 3 counts
	fnGSensorWrite(MMA8452_I2C_BUS, MMA8452_I2C_ADDRESS, REG_MT_THS, &buf[2], 1);

	//	Set the debounce counter to eliminate false positive readings for 50Hz sample rate with a requirement of 120 ms timer, assuming Normal Mode.
	//	120 ms/20 ms (steps) = 6 counts
	fnGSensorWrite(MMA8452_I2C_BUS, MMA8452_I2C_ADDRESS, REG_MT_COUNT, &buf[3], 1);

	//	Enable Motion/Freefall Interrupt Function in the System
	fnGSensorWrite(MMA8452_I2C_BUS, MMA8452_I2C_ADDRESS, REG_CTRL_4, &buf[4], 1);

	//	Route the Motion/Freefall Interrupt Function to INT2 hardware pin
	fnGSensorWrite(MMA8452_I2C_BUS, MMA8452_I2C_ADDRESS, REG_CTRL_5, &buf[5], 1);

	//	Put the device in Active Mode, 50 Hz
	fnGSensorWrite(MMA8452_I2C_BUS, MMA8452_I2C_ADDRESS, REG_CTRL_1, &buf[6], 1);

	return;
}
//------------------------------------------------
