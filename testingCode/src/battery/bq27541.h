#ifndef _BQ27541_H_
#define _BQ27541_H_

#include "batteryQueue.h"
#include "battery.h"
#include "batteryComm.h"

#define	BQ27541_I2C_BUS			"/dev/i2c-2"		//	Gas Guage I2C Bus
#define BQ27541_I2C_ADDRESS		0x55				//	Gas Guage Slave Address

//	Gas Gauge Parameter Unit
#define BQ27541_TEMP_UNIT		10	    			//	0.1K
#define BQ27541_VOLTAGE_UNIT	1000				//	mV
#define BQ27541_CURRENT_UNIT	1000				//	mAh

//	Control Status Bist Definistions
//	|===========|===========|===========|===========|===========|===========|===========|===========|===========|
//	|           |    Bit7   |    Bit6   |    Bit5   |    Bit4   |    Bit3   |    Bit2   |    Bit1   |    Bit0   |
//	|===========|===========|===========|===========|===========|===========|===========|===========|===========|
//  | High Byte |     SE    |    FAS    |     SS    |    CSV    |    CCA    |    BCA    |      -    |      -    |
//  |===========|===========|===========|===========|===========|===========|===========|===========|===========|
//  | Low Byte  | SHUTDOWN  | HIBERNATE | FULLSLEEP |   SLEEP   |    LDMD   |  RUP_DIS  |     VOK   |     QEN   |
//  |===========|===========|===========|===========|===========|===========|===========|===========|===========|
//
//	SE = Status bit indicating the SE pin is active. True when set (i.e. SE pin is low) . Default is 0
//	FAS = Status bit indicating the bq27541 is in FULL ACCESS SEALED state. Active when set.
//	SS = Status bit indicating the bq27541 is in the SEALED State. Active when set.
//	CSV = Status bit indicating a valid data flash checksum has been generated. Active when set.
//	CCA = Status bit indicating the bq27541 Coulomb Counter Calibration routine is active. Active when set.
//	BCA = Status bit indicating the bq27541 Board Calibration routine is active. Active when set.
//	SHUTDOWN = Control bit indicating the fuel gauge can force its SE pin low to signal an external shutdown. True when set. Default is 0.
//	HIBERNATE = Status bit indicating a request for entry into HIBERNATE from SLEEP mode has been issued. True when set. Default is 0.
//	Control bit when set will put the bq27541 into the lower power state of SLEEP mode. It is not possible to monitor this bit
//	FULLSLEEP = because any communication will automatically clear it. The state can be detected by monitoring the power used by the bq27541.
//	SLEEP = Status bit indicating the bq27541 is in SLEEP mode. True when set
//	LDMD = Status bit indicating the bq27541 Impedance Track? algorithm using constant-power mode. True when set. Default is 0 (constant-current mode).
//	RUP_DIS = Status bit indicating the bq27541 Ra table updates are disabled. True when set.
//	VOK = Status bit indicating cell voltages are OK for Qmax updates. True when set.
//	QEN = Status bit indicating the bq27541 Qmax updates are enabled. True when set.

//	BQ27541 Register Address
#define BQ27541_REG_CNTL		0x00				//	Control
#define BQ27541_REG_TEMP		0x06				//	Temperature
#define BQ27541_REG_CV			0x08				//	Charge Voltage
#define BQ27541_REG_FLAGS		0x0A				//	Status
#define BQ27541_REG_AI			0x14				//	Average Currtn Low Byte
#define BQ27541_REG_TTE			0x16				//	Time to Empty in Discharge
#define BQ27541_REG_TTF			0x18				//	Time to Full
#define BQ27541_REG_CC			0x2A				//	Cycle Count
#define BQ27541_REG_SOC			0x2C				//	State of Charge
#define BQ27541_REG_DNAMELEN	0x62				//	Device Name Length
#define BQ27541_REG_DNAME		0x63				//	Device Name

//	Flags Bit Definitions of High & Low Byte
#define BQ27541_FB_OTC			(1<<15)				//	Over-Temperature in Charge condition is detected
#define BQ27541_FB_OTD      	(1<<14)				//	Over-Temperature in Discharge condition is detected
#define BQ27541_FB_CHG_INH  	(1<<11)				//	Charge Inhibit indicates the temperature is outside the range [Charge Inhibit Temp Low, Charge Inhibit Temp High]
#define BQ27541_FB_XCHG     	(1<<10) 			//	Charge Suspend Alert indicates the temperature is outside the range [Suspend Temperature Low, Suspend Temperature High]
#define BQ27541_FB_FC       	(1<<9)  			//	Full-charged condition reached (RMFCC=1; Set FC_Set%=-1% when RMFCC=0)
#define BQ27541_FB_CHG      	(1<<8)  			//	(Fast) charging allowed
#define BQ27541_FB_SOC1			(1<<2)  			//	State-of-Charge-Threshold 1 (SOC1 Set) reached
#define BQ27541_FB_SOCF     	(1<<1)  			//	State-of-Charge-Threshold Final (SOCF Set %) reached
#define BQ27541_FB_DSG      	(1<<0)  			//	Discharging detected

extern BATTERY_FUNC_T bq27541;

#endif	//	_BQ27541_H_
