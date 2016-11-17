#ifndef _BATTERY_H_
#define _BATTERY_H_

#include "batteryQueue.h"

/********** Battery Register Format : aadd **********/
//	Battery Reigster Length
#define	BATTERY_REG_LEN			2
#define BATTERY_DATA_LEN		2

//	Battery Register Start Address
#define	BATTERY_REG_STR			0
#define	BATTERY_DATA_STR		(BATTERY_REG_STR + BATTERY_REG_LEN)

/********** End of RTC Register Format **********/

//	Stack Size
#define	BATTERY_STACK_SIZE			0x100000

//	Sleep
#define BATTERY_MILLISEC_BASE		(1000)
#define BATTERY_MICROSEC_BASE		(1000000)
#define	BATTERY_SLEEP_TIME			(10 * BATTERY_MILLISEC_BASE)		//	10ms

//	Battery Criterion
#define CAPCITY_MIN			20
#define CAPCITY_MAX			100
#define TEMPERATURE_MIN		300
#define TEMPERATURE_MAX		400
#define CURRENT_MIN			0
#define CURRENT_MAX			2000000
#define VOLTAGE_MIN			0
#define VOLTAGE_MAX			4048000

//	Battery Access Function Pointer
typedef struct
{
	u8 (*pBatteryGet)		(BATTERY_QUQEUE_T *battery);			//	Get RTC
	void (*pBatterySet)		(void);									//	Set RTC
	void (*pRegRead)		(u8 reg, u8 *data);						//	Read Register
	void (*pRegWrite)		(u8 reg, u8 data);						//	Write Register

}	BATTERY_FUNC_T;

//	Battery Command
typedef enum
{
	BC_START = 0x01,
	BC_DBG,
	BC_RETURN,
	
}	BATTERY_COMMAND_T;

//	Battery Debug Command
typedef enum
{
	BD_REG_READ = 0x01,
	BD_REG_WRITE,
	BD_RETURN,
	
}	BATTERY_DEBUG_COMMAND_T;

//	Battery Device Handle
typedef struct
{
	BATTERY_FUNC_T 		*batteryFunc;								//	RTC Access Function
	BATTERY_QUQEUE_T	battery;									//	RTC Queue

}	BATTERY_DEVICE_HANDLE;

//	Function
void fnBatteryInit(void);
u8   fnBatteryProcess(u8 cmd);

#endif	//	_BATTERY_H_
