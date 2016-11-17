#ifndef _MMA8452_H_
#define _MMA8452_H_

#include <math.h>
#include "common.h"
#include "gsensor.h"
#include "gsensorComm.h"

#define	MMA8452_I2C_BUS				"/dev/i2c-0"	//	MMA8452 I2C Bus
#define MMA8452_I2C_ADDRESS			0x1c			//	MMA8452 Slave Address

//	Gravity UNIT
#define GRAVITY_MILLI_BASE			(1000)
#define GRAVITY_MICRO_BASE			(1000000)
#define	GRAVITY_UINT				(GRAVITY_MILLI_BASE)

//	MMA8452 Register Map
#define	REG_STATUS					0x00			//	Real time status
#define REG_OUT_X_MSB				0x01			//	[7:0] are 8 MSBs of 12-bit sample
#define REG_OUT_X_LSB   			0x02			//	[7:4] are 4 LSBs of 12-bit sample.
#define REG_OUT_Y_MSB   			0x03			//	[7:0] are 8 MSBs of 12-bit sample
#define REG_OUT_Y_LSB   			0x04            //	[7:4] are 4 LSBs of 12-bit sample.
#define REG_OUT_Z_MSB   			0x05            //  [7:0] are 8 MSBs of 12-bit sample
#define REG_OUT_Z_LSB   			0x06            //	[7:4] are 4 LSBs of 12-bit sample.
#define REG_SYSMOD	 				0x0B            //	Current System Mode
#define REG_INT_SOURCE				0x0C            //	Interrupt status
#define REG_WHO_AM_I				0x0D            //	Device ID (0x2A)
#define REG_XYZ_DATA_CFG			0x0E            //	HPF Data Out and Dynamic Range Settings
#define REG_HP_FILTER_CUTOFF 		0x0F            //	Cut-off frequency is set to 16 Hz @ 800 Hz
#define REG_PL_STATUS               0x10            //	Landscape/Portrait orientation status
#define REG_PL_CFG					0x11            // 	Landscape/Portrait configuration
#define REG_PL_COUNT                0x12            //	Landscape/Portrait debounce counter
#define REG_PL_BF_ZCOMP             0x13            //	Back-Front, Z-Lock Trip threshold
#define REG_P_L_THS                 0x14            //	Portrait to Landscape Trip Angle is 29¢X
#define REG_MT_CFG                  0x15            //	Freefall/Motion functional block configuration
#define REG_MT_SRC                  0x16            //	Freefall/Motion event source register
#define REG_MT_THS                  0x17            //	Freefall/Motion threshold register
#define REG_MT_COUNT                0x18            //	Freefall/Motion debounce counter
#define REG_TS_CFG                  0x1D            //	Transient functional block configuration
#define REG_TS_SRC                  0x1E            //	Transient event status register
#define REG_TS_THS                  0x1F            //	Transient event threshold
#define REG_TS_COUNT                0x20            //	Transient debounce counter
#define REG_PULSE_CFG               0x21            //	ELE, Double_XYZ or Single_XYZ
#define REG_PLUSE_SRC               0x22            //	EA, Double_XYZ or Single_XYZ
#define REG_PULSE_THSX              0x23            //	X pulse threshold
#define REG_PLUSE_THSY              0x24            //	Y pulse threshold
#define REG_PLUSE_THSZ              0x25            //	Z pulse threshold
#define REG_PLUSE_TMLT              0x26            //	Time limit for pulse
#define REG_PLUSE_LTCY              0x27            //	Latency time for 2nd pulse
#define REG_PLUSE_WIND              0x28            //	Window time for 2nd pulse
#define REG_ASLP_COUNT              0x29            //	Counter setting for Auto-SLEEP
#define REG_CTRL_1                  0x2A            //	ODR = 800 Hz, STANDBY Mode.
#define REG_CTRL_2                  0x2B            //	Sleep Enable, OS Modes, RST, ST
#define REG_CTRL_3                  0x2C            //	Wake from Sleep, IPOL, PP_OD
#define REG_CTRL_4                  0x2D            //	Interrupt enable register
#define REG_CTRL_5                  0x2E            //	Interrupt pin (INT1/INT2) map
#define REG_OFF_X                   0x2F            //	X-axis offset adjust
#define REG_OFF_Y                   0x30            //	Y-axis offset adjust
#define REG_OFF_Z                   0x31            //	Z-axis offset adjust

//	GSensor DEVICE ID
#define MMA8452_DEV_ID				0x2A

//	GSensor Output Calculate
#define GOUT_CALCULATE(a,b)			((a<<4) + (b>>4))

//	Full Scale Range
//|==============================|
//|	FS1 | FS2 | Full Scale Range |
//|==============================|
//|	 0  |  0  |        2         |
//|	 0  |  1  |        4         |
//|	 1  |  0  |        8         |
//|	 1  |  1  |     Reserved     |
//|===============================
#define FS_RANGE					(3 << 1)			//	Full Scale
#define FS_RANGE_CONVERT(a)			pow(2,(1+a))		//	Full Scale Convert

//	Fast Read  Mode
//|=================================================|
//|	F_READ |   Data Format Limited to Single Byte   |
//|=================================================|
//|	   0   |          Normal Mode (12-bit)          |
//|	   1   |          Normal Mode (8-bit)           |
//|=================================================|
#define FS_READ						(1 < 1)				//	Fast Read

#define FS_READ_MODE(a)	\
		((a==0)?12:		\
		((a==1)?8 :0))

//	Accelerometer Output Data for 12-Bit
//|===================================================================|
//|	  12-bit Data  |  Range 2g(1mg) |  Range 4g(2mg) | Range 8g(3.9mg)|
//|===================================================================|
//|	0111 1111 1111 |     +1.999g    |     +3.998g    |    +7.996g     |
//|===================================================================|
//|	0111 1111 1110 |     +1.998g    |     +3.996g    |    +7.992g     |
//|===================================================================|
//|       ...      |       ...      |       ...      |       ...      |
//|===================================================================|
//|	0000 0000 0001 |     +0.001g    |     +0.002g    |    +0.0004g    |
//|===================================================================|
//|	0000 0000 0000 |     +0.000g    |     +0.000g    |    +0.0000g    |
//|===================================================================|
//|	1111 1111 1111 |     -0.001g    |     -0.002g    |    -0.0004g    |
//|===================================================================|
//|       ...      |       ...      |       ...      |       ...      |
//|===================================================================|
//|	1000 0000 0001 |     -1.999g    |     -3.998g    |    -7.996g     |
//|===================================================================|
//|	1000 0000 0000 |     -2.000g    |     -4.000g    |    -8.0000g    |
//|===================================================================|


//	Accelerometer Output Data for 8-Bit
//|===========================================================================|
//|	    8-bit Data   | Range 2g(15.6mg) | Range 4g(31.25mg)| Range 8g(62.5mg) |
//|===========================================================================|
//|	    0111 1111    |      +1.9844g    |       +3.9688g   |     +7.9375g     |
//|===========================================================================|
//|	    0111 1110    |      +1.9688g    |       +3.9375g   |     +7.8750g     |
//|===========================================================================|
//|        ...       |        ...       |        ...       |        ...       |
//|===========================================================================|
//|	    0000 0001    |      +0.0156g    |       +0.0313g   |     +0.0625g     |
//|===========================================================================|
//|	    0000 0000    |      +0.000g     |       +0.000g    |     +0.000g      |
//|===========================================================================|
//|	    1111 1111    |      -0.0156g    |       -0.0313g   |     -0.0625g     |
//|===========================================================================|
//|        ...       |        ...       |        ...       |        ...       |
//|===========================================================================|
//|	    1000 0001    |      -1.9844g    |       -3.9688g   |     -7.9375g     |
//|===========================================================================|
//|	    1000 0000    |      -2.0000g    |       -4.0000g   |     -8.0000g     |
//|===========================================================================|

#define NORMAL_COMPLEMENT_CHECK		(1<<11)			//	2's Complement Number for Normal Mode
#define FASTREAD_COMPLEMENT_CHECK	(1<<7)			//	2's Complement Number for Normal Mode

//	Landscape to Portrait Screen Orientation Change Positions
//			 													Back
//			 PU			 LL    	    PD         LR    	-----------------------
//	|    |--------|  |--------| |--------| |--------|         |--------|
//	|    | o      |  |        | |        | |       o|
//	| 1g |        |  | o      | |       o| |        |   		Front
//	¡õ    |--------|  |--------| |--------| |--------|		  |--------|
//														-----------------------

//	Offset Value for All Six Faces
//	|-------------|-------------|---------------|----------------|-----------------|-------|------|
//  | 6 Positions | Portrait Up | Portrait Down | Landscape Left | Landscape Right | Front | Back |
//	|-------------|-------------|---------------|----------------|-----------------|-------|------|
//  |		X     |      0g     |      -1g      |        0g      |        +1g      |   0g  |  0g  |
//	|-------------|-------------|---------------|----------------|-----------------|-------|------|
//  |		Y     |     -1g     |       0g      |       +1g      |         0g      |   0g  |  0g  |
//	|-------------|-------------|---------------|----------------|-----------------|-------|------|
//  |		Z     |      0g     |       0g      |        0g      |         0g      |  +1g  | -1g  |
//	|-------------|-------------|---------------|----------------|-----------------|-------|------|

//	Offset Value
typedef struct
	{
		s16 pu;		//	Portrait Up
		s16 pd;		//	Portrait Down
		s16 ll;		//	Landscape Left
		s16 lr;		//	Landscape Right
		s16 front;	//	Front
		s16 back;	//	Back

	}	MMA8452Q_T;


extern GSENSOR_FUNC_T mma8452;

#endif	//	_MMA8452_H_
