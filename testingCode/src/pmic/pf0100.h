#ifndef _PF0100_H_
#define _PF0100_H_

#include "common.h"
#include "pmic.h"

//	PF0100 I2C Information
#define	PF0100_I2C_BUS				"/dev/i2c-1"		//	PF0100 I2C Bus
#define PF0100_I2C_ADDRESS			0x08				//	PF0100 Slave Address

//	PF0100 Device Identification
#define PF0100_ID					0x10				//	Device ID

//	PF0100 Register Table
#define REG_PF0100_DEVID			0x00				//	Device ID
#define REG_PF0100_COINCTL			0x1A				//	Coin Cell Charger

/********** SW1ABC Register Summary **********/
#define REG_PF0100_SW1ABVOLT		0x20				//	SW1AB Output voltage set point in normal operation
#define REG_PF0100_SW1ABSTBY		0x21				//	SW1AB Output voltage set point on Standby
#define REG_PF0100_SW1ABOFF			0x22				//	SW1AB Output voltage set point on Sleep
#define REG_PF0100_SW1ABMODE		0x23				//	SW1AB Switching Mode selector register
#define REG_PF0100_SW1ABCONF		0x24				//	SW1AB DVS, Phase, Frequency and ILIM configuration
#define REG_PF0100_SW1CVOLT			0x2E				//	SW1C Output voltage set point in normal operation
#define REG_PF0100_SW1CSTBY			0x2F				//	SW1C Output voltage set point in Standby
#define REG_PF0100_SW1COFF			0x30				//	SW1C Output voltage set point on Sleep
#define REG_PF0100_SW1CMODE			0x31				//	SW1C Switching Mode selector register
#define REG_PF0100_SW1CCONF			0x32				//	SW1C DVS, Phase, Frequency and ILIM configuration
#define REG_SW1ABMODE(x)			(x & 0x0f)			//	SW1AB Switch Operation Mode
#define REG_SW1ABOMODE(x)			((x & 0x20) >> 4)	//	SW1AB Sleep Mode
#define REG_SW1CMODE(x)				(x & 0x0f)			//	SW1C Switch Operation Mode
#define REG_SW1COMODE(x)			((x & 0x20) >> 4)	//	SW1C Sleep Mode

/********** SW2 Register Summary **********/
#define REG_PF0100_SW2VOLT 			0x35 				//	SW2 Output voltage set point on normal operation
#define REG_PF0100_SW2STBY 			0x36 				//	SW2 Output voltage set point on Standby
#define REG_PF0100_SW2OFF 			0x37 				//	SW2 Output voltage set point on Sleep
#define REG_PF0100_SW2MODE 			0x38 				//	SW2 Switching Mode selector register
#define REG_PF0100_SW2CONF 			0x39 				//	SW2 DVS, Phase, Frequency, and ILIM configuration
#define REG_SW2MODE(x)				(x & 0x0f)			//	SW2 Switch Operation Mode
#define REG_SW2OMODE(x)				((x & 0x20) >> 4)	//	SW2 Sleep Mode

/********** SW3AB Register Summary **********/
#define REG_PF0100_SW3AVOLT 		0x3C 				//	SW3A Output voltage set point on normal operation
#define REG_PF0100_SW3ASTBY 		0x3D 				//	SW3A Output voltage set point on Standby
#define REG_PF0100_SW3AOFF 			0x3E 				//	SW3A Output voltage set point on Sleep
#define REG_PF0100_SW3AMODE 		0x3F 				//	SW3A Switching mode selector register
#define REG_PF0100_SW3ACONF 		0x40 				//	SW3A DVS, phase, frequency and ILIM configuration
#define REG_PF0100_SW3BVOLT 		0x43 				//	SW3B Output voltage set point on normal operation
#define REG_PF0100_SW3BSTBY 		0x44 				//	SW3B Output voltage set point on Standby
#define REG_PF0100_SW3BOFF 			0x45 				//	SW3B Output voltage set point on Sleep
#define REG_PF0100_SW3BMODE 		0x46 				//	SW3B Switching mode selector register
#define REG_PF0100_SW3BCONF 		0x47 				//	SW3B DVS, phase, frequency and ILIM configuration
#define REG_SW3AMODE(x)				(x & 0x0f)			//	SW3A Switch Operation Mode
#define REG_SW3AOMODE(x)			((x & 0x20) >> 4)	//	SW3A Sleep Mode
#define REG_SW3BMODE(x)				(x & 0x0f)			//	SW3B Switch Operation Mode
#define REG_SW3BOMODE(x)			((x & 0x20) >> 4)	//	SW3B Sleep Mode

/********** SW4 Register Summary **********/
#define REG_PF0100_SW4VOLT 			0x4A 				//	SW4 Output voltage set point on normal operation
#define REG_PF0100_SW4STBY 			0x4B 				//	SW4 Output voltage set point on Standby
#define REG_PF0100_SW4OFF 			0x4C 				//	SW4 Output voltage set point on Sleep
#define REG_PF0100_SW4MODE 			0x4D 				//	SW4 Switching mode selector register
#define REG_PF0100_SW4CONF 			0x4E 				//	SW4 DVS, phase, frequency and ILIM configuration
#define REG_SW4BMODE(x)				(x & 0x0f)			//	SW4 Switch Operation Mode
#define REG_SW4BOMODE(x)			((x & 0x20) >> 4)	//	SW4 Sleep Mode

/********** Boost Regulator Summary **********/
#define REG_PF0100_SWBSTCTL 		0x66 				//	Boost Regulator Contorl
#define REG_BST_VOL(x)				(x & 0x03)			//	SWBST Voltage
#define REG_BST_NORMAL(x)			((x & 0x0C) >> 2)	//	SWBST Normal operation Mode
#define REG_BST_STANDBY(x)			((x & 0x60) >> 5)	//	SWBST Standby Mode

/********** BooVSNVS Control Summary **********/
#define REG_PF0600_VSNVSCTL			0x6B				//	VSNVS Control Register
#define REG_VSNVS_VOL(x)			(x & 0x07)			//	VSNVS Voltage

/********** LDO Regulator Summary **********/
#define REG_PF0100_VGEN1CTL			0x6C				//	VGENl Control Register
#define REG_PF0100_VGEN2CTL			0x6D				//	VGEN2 Control Register
#define REG_PF0100_VGEN3CTL			0x6E				//	VGEN3 Control Register
#define REG_PF0100_VGEN4CTL			0x6F				//	VGEN4 Control Register
#define REG_PF0100_VGEN5CTL			0x70				//	VGEN5 Control Register
#define REG_PF0100_VGEN6CTL			0x71				//	VGEN6 Control Register

#define REG_VGENx_VOLT(x)			(x & 0x0f)			//	VGEN Output Voltage
#define REG_VGENx_EN(x)				((x & 0x10) >> 4 )	//	VGEN Output Enable/ Disable
#define REG_VGENx_STBY(x)			((x & 0x20) >> 5 )	//	VGEN Output Standby Flag
#define REG_VGENx_LPWR(x)			((x & 0x40) >> 6 )	//	VGEN Output Low Power Flag

/********** On Time Program SW1AB Configuration **********/
#define REG_PF0100_OPT_SW1AB_CONFIG 0xA2				//	SW1AB Configuration Register
#define REG_SW1_CONFIG(x)			((x & 0x0c) >> 2)	//	SW1 Configuration
#define REG_SW1AB_FREQ(x) 			(x & 0x03)			//	SW1AB Frequence


#define REG_PF0600_OPT_SW3A_CONFIG 	0xB2				//	SW3A Configuration Register
#define REG_SW3_CONFIG(x)			((x & 0x0c) >> 2)	//	SW3 Configuration
#define REG_SW3A_FREQ(x) 			(x & 0x03)			//	SW3A Frequence

//	Buck Port Number
typedef enum
{
	BUCK_PORT_1 = 1,
	BUCK_PORT_2,
	BUCK_PORT_3,
	BUCK_PORT_4,

}	BUCK_PORT_T;

//	Buck 1 Setpoint
typedef enum
{
	BUCK1_SETPOINT_0 = 0,
	BUCK1_SETPOINT_63 = 63,

}	BUCK1_OUTPUT_T;

//	Buck 2 Setpoint
typedef enum
{
	BUCK2_SETPOINT_0 = 0,
	BUCK2_SETPOINT_114 = 114,

}	BUCK2_OUTPUT_T;

//	Buck 3 Setpoint
typedef enum
{
	BUCK3_SETPOINT_0 = 0,
	BUCK3_SETPOINT_114 = 114,

}	BUCK3_OUTPUT_T;

//	Buck 4 Setpoint
typedef enum
{
	BUCK4_SETPOINT_0 = 0,
	BUCK4_SETPOINT_114 = 114,

}	BUCK4_OUTPUT_T;

//	Boost Voltage Set Point
typedef enum
{
	BST_SETPOINT_0 = 0,
	BST_SETPOINT_3 = 3,

}	BOOST_OUTPUT_T;

//	Boost Switch Mode
typedef enum
{
	BOOST_SWITCH_0 = 0,
	BOOST_SWITCH_3 = 3,

}	BOOST_SWITCH_T;

//	LDO Port Numer
typedef enum
{
	LDO_PORT_1 = 1,
	LDO_PORT_2,
	LDO_PORT_3,
	LDO_PORT_4,
	LDO_PORT_5,
	LDO_PORT_6,

}	LDO_PORT_T;

//	LDO Contorl
#define LDO_DISABLE					0
#define LDO_ENABLE					1

//	LDO Voltage Set Point
typedef enum
{
	LDO_SETPOINT_0 = 0,
	LDO_SETPOINT_15 = 15,

}	LDO_OUTPUT_T;

//	SNVS Set Point
typedef enum
{
	SNVS_SETPOINT_0 = 0,
	SNVS_SETPOINT_6 = 6,
	SNVS_SETPOINT_7 = 7,

}	SNVS_OUTPUT_T;

//	PF0100 Config Table
typedef enum
{
	PCF_BUCK = 1,
	PCF_BOOST,
	PCF_LDO,
	PCF_SNVS,
	PCF_RETURN,

}	PF0100_CONFIG_T;

//	Extern pf0100 function pointer
extern PMIC_FUNC_T pf0100;

#endif // _PF0100_H_
