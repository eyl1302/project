#ifndef _EETI_H_
#define _EETI_H_

#include <fcntl.h>
#include <linux/input.h>

#include "common.h"
#include "tpanel.h"
#include "tpanelComm.h"

//	eeti I2C Information
#define	EETI_I2C_BUS				"/dev/i2c-2"		//	eeti I2C Bus
#define EETI_I2C_ADDRESS			0x04				//	eeti Slave Address

//	eeti event
#define EVENT_DEVICE    			"/dev/input/event0"
#define EVENT_TYPE      			EV_ABS
#define EVENT_CODE_X    			ABS_X
#define EVENT_CODE_Y    			ABS_Y

//	LCD Display Resolution
#define RSLN_Y_WIDTH				480
#define RSLN_Y_HEIGHT				640
#define ABSX_MIN					0
#define ABSX_MAX					32760
#define ABSY_MIN					0
#define ABSY_MAX					32760

//	Panel Resolution
#define RSLN_X_MIN					0
#define RSLN_Y_MIN					0
#define RSLN_X_HALF					((RSLN_Y_WIDTH >> 1 ) - 1)
#define RSLN_Y_HALF					((RSLN_Y_HEIGHT >> 1 ) - 1)
#define RSLN_X_MAX					(RSLN_Y_WIDTH - 1 )
#define RSLN_Y_MAX					(RSLN_Y_HEIGHT  - 1)
#define DISPLAY_WIDTH_HALF			((RSLN_Y_WIDTH >> 1 ) - 1)
#define DISPLAY_HEIGHT_HALF			((RSLN_Y_HEIGHT >> 1 ) - 1)

//	ABSX_MAX -> RSLN_X_WIDTH  => 32760/70 + 12 = 480
//	ABSY_MAX -> RSLN_Y_HEIGHT => 32760/52 + 10 = 640
#define CONTACT_CONVERT_X(x)  	((x) / 70 + 12)
#define CONTACT_CONVERT_Y(y)  	((y) / 52 + 10)

//	Touch Panel Blcok Sequence
typedef enum
{
	TB_UL = 0,					//	Up & Left
	TB_DL,						//	Down & Left
	TB_UR,						//	Up & Right
	TB_DR,						//	Down & Right

}	TOUCH_PANEL_BLOCK_T;

//	Panel Blok
#define PANEL_BOLCK(x)	\
					((x == TB_UL)? "Up & Left" : \
					((x == TB_DL)? "Down & Left" : \
					((x == TB_UR)? "Up & Right" : \
					((x == TB_DR)? "Down & Right" : "Unkonw Block"))))

//	Touch Contact
typedef struct
{
	u16	x[16];
	u16 y[16];

}	TOUCH_CONTACT_T;

//	eeti function pointer
extern TPANEL_FUNC_T eeti;

#endif // _EETI_H_
