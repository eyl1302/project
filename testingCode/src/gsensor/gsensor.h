#ifndef _GSENSOR_H_
#define _GSENSOR_H_

#include "common.h"

/********** GSensor Register Format : aadd **********/
//	GSensor Reigster Length
#define	GSENSOR_REG_LEN			2
#define GSENSOR_DATA_LEN		2

//	GSensor Register Start Address
#define	GSENSOR_REG_STR			0
#define	GSENSOR_DATA_STR		(GSENSOR_REG_STR + GSENSOR_REG_LEN)

/********** End of GSensor Register Format **********/

//	Stack Size
#define	GSENSOR_STACK_SIZE		0x100000

//	Sleep
#define GSENSOR_MILLISEC_BASE	(1000)
#define GSENSOR_MICROSEC_BASE	(1000000)
#define	GSENSOR_SLEEP_TIME		(10 * GSENSOR_MILLISEC_BASE)		//	10ms

//	GSensor Command
typedef enum
{
	GC_START = 0x01,
	GC_DBG,
	GC_RETURN,
	
}	GSENSOR_COMMAND_T;

//	GSensor Debug Command
typedef enum
{
	GD_CALIBRATION = 0x1,
	GD_REG_READ,
	GD_REG_WRITE,
	GD_RETURN,

}	GSENSOR_DEBUG_COMMAND_T;

//	GSensor Output
typedef  struct
{
	float x;														//	x Out
	float y;                                                      	//	y Out
	float z;                                                      	//	z Out

}	GSENSOR_OUTPUT;

//	Audio Access Function Pointer
typedef struct
{
	u8 (*pInit)			(u8 mode);									//	Initial Function
	u8 (*pReadOut)		(GSENSOR_OUTPUT *gsr);						//	Read X,Y,Z Out
	u8 (*pCalibration)	(u8 mode);									//	Calibration
	u8 (*pRegRead)		(u8 reg, u8 *data);							//	Read Function
	u8 (*pRegWrite)		(u8 reg, u8 data);							//	Write Function

}	GSENSOR_FUNC_T;

//	GSensor Device Handle
typedef struct
{
	GSENSOR_FUNC_T 		*gsensorFunc;								//	GSensor Access Function
	//GSENSOR_QUQEUE_T	gsensor;									//	GSensor Queue

}	GSENSOR_DEVICE_HANDLE;

void fnGSensorInit(void);
u8 	 fnGSensorProcess(u8 cmd);

#endif	//_GSENSOR_H_
