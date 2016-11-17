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
#include "pf0100.h"

//	Internal Function


//	Function Pointer
u8 fnPF0100Init(void);
u8 fnPF0100List(void);
u8 fnPF0100Config(void);
u8 fnPF0100Test(void);
u8 fnPF0100RegRead(u8 reg, u8 *data);
u8 fnPF0100RegWrite(u8 reg, u8 data);

PMIC_FUNC_T pf0100 =
{
	fnPF0100Init,
	fnPF0100List,
	fnPF0100Config,
	fnPF0100Test,
	fnPF0100RegRead,
	fnPF0100RegWrite,
};

#define SW1_OUTPUT(a) 		(float)(a * (0.025) + 0.3)		//	SW1 Normal, Standy , Sleep Mode Output Voltage
#define SW2_OUTPUT(a) 		(float)(a * (0.025) + 0.4)		//	SW2 Normal, Standy , Sleep Mode Output Voltage
#define SW3_OUTPUT(a) 		(float)(a * (0.025) + 0.4)		//	SW3 Normal, Standy , Sleep Mode Output Voltage
#define SW4_OUTPUT(a) 		(float)(a * (0.025) + 0.4)		//	SW4 Normal, Standy , Sleep Mode Output Voltage
#define SWBST_OUTPUT(a)		(float)(a * (0.050) + 5.0)		//	Boost regulator Output Voltage
#define VGEN12_OUTPUT(a) 	(float)(a * (0.050) + 0.8)		//	VGEN1, VGEN2 Output Voltage Configuration
#define VGEN3456_OUTPUT(a) 	(float)(a * (0.100) + 1.8)		//	VGEN3/ 4/ 5/ 6 Output Voltage Configuration

#define VSNVS_OUTPUT(a)		\
						((a == 0)?(float)1.0:		\
						((a == 1)?(float)1.1:		\
						((a == 2)?(float)1.2:		\
						((a == 3)?(float)1.3:		\
						((a == 4)?(float)1.5:		\
						((a == 5)?(float)1.8:		\
						((a == 6)?(float)3.0: 0.0)))))))	//	VSNVS Output Voltage Configuration

#define SW1_CONFIG(a)	\
					((a==0)?"A/B/C Single Phase":\
					((a==1)?"A/B Single Phase & C Independent mode":\
					((a==2)?"A/B Dual Phase & C Independent mode":\
					((a==3)?"Reserved":"Unknow"))))

#define SW3_CONFIG(a)	\
					((a==0)?"A/B Single Phase":\
					((a==1)?"A/B Single Phase":\
					((a==2)?"A/B Dual Phase":\
					((a==3)?"A/B Independent":"Unknow"))))

#define SWBST_MODE(a)	\
					((a==0)?"OFF":\
					((a==1)?"PFM":\
					((a==2)?"Auto":\
					((a==3)?"APS":"Unknow"))))

#define SWxMODE_NORMAL(a)	\
					((a==0)?"OFF":\
					((a==1)?"PWM":\
					((a==2)?"Reserved":\
					((a==3)?"PFM":\
					((a==4)?"APS":\
					((a==5)?"PWM":\
					((a==6)?"PWM":\
					((a==7)?"Reserved":\
					((a==8)?"APS":\
					((a==9)?"Reserved":\
					((a==10)?"Reserved":\
					((a==11)?"Reserved":\
					((a==12)?"APS":\
					((a==13)?"PWM":\
					((a==14)?"Reserved":\
					((a==15)?"Reserved":"Unknow"))))))))))))))))	//	Regulator Mode Control

#define SWxMODE_STANDBY(a)	\
					((a==0)?"OFF":\
					((a==1)?"OFF":\
					((a==2)?"Reserved":\
					((a==3)?"OFF":\
					((a==4)?"OFF":\
					((a==5)?"PWM":\
					((a==6)?"APS":\
					((a==7)?"Reserved":\
					((a==8)?"APS":\
					((a==9)?"Reserved":\
					((a==10)?"Reserved":\
					((a==11)?"Reserved":\
					((a==12)?"PFM":\
					((a==13)?"PFM":\
					((a==14)?"Reserved":\
					((a==15)?"Reserved":"Unknow"))))))))))))))))	//	Regulator Mode Control

#define SW1ABCOMODE(a)	\
					((a==0)?"OFF":\
					((a==1)?"PFM":"Unknow"))

#define SW2OMODE(a)	\
					((a==0)?"OFF":\
					((a==1)?"PFM":"Unknow"))

#define SW3OMODE(a)	\
					((a==0)?"OFF":\
					((a==1)?"PFM":"Unknow"))

#define SW4OMODE(a)	\
					((a==0)?"OFF":\
					((a==1)?"PFM":"Unknow"))

#define VGENx_EN(a)	\
					((a==0)?"OFF":\
					((a==1)?"ON":"Unknow"))

typedef struct
{
	u8 normal;			// 	Normal Operation Voltage
	u8 standby;			//	Standby Voltage
	u8 sleep;			//	Sleep Voltage
	u8 mode;			//	Regulator Mode

}	CONTROL_REG_T;

/*
 * date: 2016/10/12
 * modify : hk
 * description: Display pf0100 SW1AB Information
 */
u8 fnPF0100DisplaySW1ABC(void)
{
	u8  cfg;
	u8 	buf[8];
	CONTROL_REG_T sw1ab, sw1c;

	//	SW1A/B Configuration
	if(fnPMICRead(PF0100_I2C_BUS, PF0100_I2C_ADDRESS, REG_PF0100_OPT_SW1AB_CONFIG , buf, 1) != HK_SUCCESS)
		return HK_FAIL;

	//	Configuration
	cfg = REG_SW1_CONFIG(buf[0]);

	//	SW1A/B Output Voltage
	{
		u8 i;

		for(i=0;i<5;i++)
		{
			if(fnPMICRead(PF0100_I2C_BUS, PF0100_I2C_ADDRESS, (REG_PF0100_SW1ABVOLT + i), &buf[i], 1) != HK_SUCCESS)
				return HK_FAIL;

			//printf("\n  ### hk_dbg, %s :addr:0x%02x, data:0x%02x!! \n", __FUNCTION__, (REG_PF0100_SW1ABVOLT + i), buf[i]);
		}
	}

	sw1ab.normal  = buf[0];
	sw1ab.standby = buf[1];
	sw1ab.sleep   = buf[2];
	sw1ab.mode    = buf[3];

	//	SW1C Output Voltage
	{
		u8 i;

		for(i=0;i<5;i++)
		{
			if(fnPMICRead(PF0100_I2C_BUS, PF0100_I2C_ADDRESS, (REG_PF0100_SW1CVOLT + i), &buf[i], 1) != HK_SUCCESS)
				return HK_FAIL;

			//printf("\n  ### hk_dbg, %s :addr:0x%02x, data:0x%02x!! \n", __FUNCTION__, (REG_PF0100_SW1CVOLT + i), buf[i]);
		}
	}

	sw1c.normal  = buf[0];
	sw1c.standby = buf[1];
	sw1c.sleep   = buf[2];
	sw1c.mode    = buf[3];

	// SW1  Information Display
	printf("\n");
	printf("\n  ### |------------------------------------------------------------|");
	printf("\n  ### | SW1A/B/C Contorl List                                      |");
	printf("\n  ### |------------------------------------------------------------|");
	printf("\n  ### | SW1A/B Mode     | Normal:%s, Standby:%s, Sleep:%s       |", SWxMODE_NORMAL(REG_SW1ABMODE(sw1ab.mode)), SWxMODE_STANDBY(REG_SW1ABMODE(sw1ab.mode)), SW1ABCOMODE(REG_SW1ABOMODE(sw1ab.mode)));
	printf("\n  ### |------------------------------------------------------------|");
	printf("\n  ### | SW1C Mode       | Normal:%s, Standby:%s, Sleep:%s       |", SWxMODE_NORMAL(REG_SW1CMODE(sw1c.mode)), SWxMODE_STANDBY(REG_SW1CMODE(sw1c.mode)), SW1ABCOMODE(REG_SW1COMODE(sw1c.mode)));
	printf("\n  ### |------------------------------------------------------------|");
	printf("\n  ### | Configuration   | %s                       |", SW1_CONFIG(cfg));
	printf("\n  ### |------------------------------------------------------------|");
	printf("\n  ### | Normal Voltage  | SW1A:%1.3fV, SW1B:%1.3fV, SW1C:%1.3fV    |", SW1_OUTPUT(sw1ab.normal), SW1_OUTPUT(sw1ab.normal), SW1_OUTPUT(sw1c.normal));
	printf("\n  ### |------------------------------------------------------------|");
	printf("\n  ### | Standby Voltage | SW1A:%1.3fV, SW1B:%1.3fV, SW1C:%1.3fV    |", SW1_OUTPUT(sw1ab.standby), SW1_OUTPUT(sw1ab.standby), SW1_OUTPUT(sw1c.standby));
	printf("\n  ### |------------------------------------------------------------|");
	printf("\n  ### | Sleep Voltage   | SW1A:%1.3fV, SW1B:%1.3fV, SW1C:%1.3fV    |", SW1_OUTPUT(sw1ab.sleep), SW1_OUTPUT(sw1ab.sleep), SW1_OUTPUT(sw1c.sleep));
	printf("\n  ### |------------------------------------------------------------|");

	return HK_SUCCESS;
}
//------------------------------------------------

/*
 * date: 2016/10/12
 * modify : hk
 * description: Display pf0100 SW2 Information
 */
u8 fnPF0100DisplaySW2(void)
{
	u8 	buf[8];
	CONTROL_REG_T sw2;

	//	SW2 Output Voltage
	{
		u8 i;

		for(i=0;i<5;i++)
		{
			if(fnPMICRead(PF0100_I2C_BUS, PF0100_I2C_ADDRESS, (REG_PF0100_SW2VOLT + i), &buf[i], 1) != HK_SUCCESS)
			return HK_FAIL;

			//printf("\n  ### hk_dbg, %s :addr:0x%02x, data:0x%02x!! \n", __FUNCTION__, (REG_PF0100_SW2VOLT + i), buf[i]);
		}
	}

	sw2.normal  = buf[0];
	sw2.standby = buf[1];
	sw2.sleep   = buf[2];
	sw2.mode    = buf[3];

	// SW2  Information Display
	printf("\n");
	printf("\n  ### |------------------------------------------------------------|");
	printf("\n  ### | SW2      Contorl List                                      |");
	printf("\n  ### |------------------------------------------------------------|");
	printf("\n  ### | SW1A/B Mode     | Normal:%s, Standby:%s, Sleep:%s       |", SWxMODE_NORMAL(REG_SW2MODE(sw2.mode)), SWxMODE_STANDBY(REG_SW2MODE(sw2.mode)), SW2OMODE(REG_SW2OMODE(sw2.mode)));
	printf("\n  ### |------------------------------------------------------------|");
	printf("\n  ### | Configuration   | Single Phase                             |");
	printf("\n  ### |------------------------------------------------------------|");
	printf("\n  ### | Normal Voltage  | SW2:%1.3fV                               |", SW2_OUTPUT(sw2.normal));
	printf("\n  ### |------------------------------------------------------------|");
	printf("\n  ### | Standby Voltage | SW2:%1.3fV                               |", SW2_OUTPUT(sw2.standby));
	printf("\n  ### |------------------------------------------------------------|");
	printf("\n  ### | Sleep Voltage   | SW2:%1.3fV                               |", SW2_OUTPUT(sw2.sleep));
	printf("\n  ### |------------------------------------------------------------|");

	return HK_SUCCESS;
}
//------------------------------------------------

/*
 * date: 2016/10/12
 * modify : hk
 * description: Display pf0100 SW3AB Information
 */
u8 fnPF0100DisplaySW3AB(void)
{
	u8  cfg;
	u8 	buf[8];
	CONTROL_REG_T sw3a, sw3b;

	//	SW3A/B Configuration
	if(fnPMICRead(PF0100_I2C_BUS, PF0100_I2C_ADDRESS, REG_PF0600_OPT_SW3A_CONFIG, buf, 1) != HK_SUCCESS)
		return HK_FAIL;

	//	Configuration
	cfg = REG_SW3_CONFIG(buf[0]);

	//	SW3A Output Voltage
	{
		u8 i;

		for(i=0;i<5;i++)
		{
			if(fnPMICRead(PF0100_I2C_BUS, PF0100_I2C_ADDRESS, (REG_PF0100_SW3AVOLT + i), &buf[i], 1) != HK_SUCCESS)
				return HK_FAIL;

			//printf("\n  ### hk_dbg, %s :addr:0x%02x, data:0x%02x!! \n", __FUNCTION__, (REG_PF0100_SW3AVOLT + i), buf[i]);
		}
	}

	sw3a.normal  = buf[0];
	sw3a.standby = buf[1];
	sw3a.sleep   = buf[2];
	sw3a.mode    = buf[3];

	//	SW3B Output Voltage
	{
		u8 i;

		for(i=0;i<5;i++)
		{
			if(fnPMICRead(PF0100_I2C_BUS, PF0100_I2C_ADDRESS, (REG_PF0100_SW3BVOLT + i), &buf[i], 5) != HK_SUCCESS)
				return HK_FAIL;

			//printf("\n  ### hk_dbg, %s :addr:0x%02x, data:0x%02x!! \n", __FUNCTION__, (REG_PF0100_SW3BVOLT + i), buf[i]);
		}
	}

	sw3b.normal  = buf[0];
	sw3b.standby = buf[1];
	sw3b.sleep   = buf[2];
	sw3b.mode    = buf[3];

	// SW3 Information Display
	printf("\n");
	printf("\n  ### |------------------------------------------------------------|");
	printf("\n  ### | SW3A/B   Contorl List                                      |");
	printf("\n  ### |------------------------------------------------------------|");
	printf("\n  ### | SW3A Mode       | Normal:%s, Standby:%s, Sleep:%s       |", SWxMODE_NORMAL(REG_SW3AMODE(sw3a.mode)), SWxMODE_STANDBY(REG_SW3AMODE(sw3a.mode)), SW3OMODE(REG_SW3AOMODE(sw3a.mode)));
	printf("\n  ### |------------------------------------------------------------|");
	printf("\n  ### | SW3B Mode       | Normal:%s, Standby:%s, Sleep:%s       |", SWxMODE_NORMAL(REG_SW3BMODE(sw3b.mode)), SWxMODE_STANDBY(REG_SW3BMODE(sw3b.mode)), SW3OMODE(REG_SW3BOMODE(sw3b.mode)));
	printf("\n  ### |------------------------------------------------------------|");
	printf("\n  ### | Configuration   | %s                       |", SW1_CONFIG(cfg));
	printf("\n  ### |------------------------------------------------------------|");
	printf("\n  ### | Normal Voltage  | SW3A:%1.3fV, SW3B:%1.3fV                 |", SW3_OUTPUT(sw3a.normal), SW3_OUTPUT(sw3b.normal));
	printf("\n  ### |------------------------------------------------------------|");
	printf("\n  ### | Standby Voltage | SW3A:%1.3fV, SW3B:%1.3fV                 |", SW3_OUTPUT(sw3a.standby), SW3_OUTPUT(sw3b.standby));
	printf("\n  ### |------------------------------------------------------------|");
	printf("\n  ### | Sleep Voltage   | SW3A:%1.3fV, SW3B:%1.3fV                 |", SW3_OUTPUT(sw3a.sleep), SW3_OUTPUT(sw3b.sleep));
	printf("\n  ### |------------------------------------------------------------|");

	return HK_SUCCESS;
}
//------------------------------------------------

/*
 * date: 2016/10/12
 * modify : hk
 * description: Display pf0100 SW4 Information
 */
u8 fnPF0100DisplaySW4(void)
{
	u8 	buf[8];
	CONTROL_REG_T sw4;

	//	SW4 Output Voltage
	{
		u8 i;

		for(i=0;i<5;i++)
		{
			if(fnPMICRead(PF0100_I2C_BUS, PF0100_I2C_ADDRESS, (REG_PF0100_SW4VOLT + i), &buf[i], 5) != HK_SUCCESS)
				return HK_FAIL;

			//printf("\n  ### hk_dbg, %s :addr:0x%02x, data:0x%02x!! \n", __FUNCTION__, (REG_PF0100_SW4VOLT + i), buf[i]);
		}
	}

	sw4.normal  = buf[0];
	sw4.standby = buf[1];
	sw4.sleep   = buf[2];
	sw4.mode    = buf[3];

	// SW4  Information Display
	printf("\n");
	printf("\n  ### |------------------------------------------------------------|");
	printf("\n  ### | SW4 Contorl List                                           |");
	printf("\n  ### |------------------------------------------------------------|");
	printf("\n  ### | SW4 Mode        | Normal:%s, Standby:%s, Sleep:%s       |", SWxMODE_NORMAL(REG_SW4BMODE(sw4.mode)), SWxMODE_STANDBY(REG_SW4BMODE(sw4.mode)), SW2OMODE(REG_SW4BOMODE(sw4.mode)));
	printf("\n  ### |------------------------------------------------------------|");
	printf("\n  ### | Configuration   | Single Phase                             |");
	printf("\n  ### |------------------------------------------------------------|");
	printf("\n  ### | Normal Voltage  | SW4:%1.3fV                               |", SW4_OUTPUT(sw4.normal));
	printf("\n  ### |------------------------------------------------------------|");
	printf("\n  ### | Standby Voltage | SW4:%1.3fV                               |", SW4_OUTPUT(sw4.standby));
	printf("\n  ### |------------------------------------------------------------|");
	printf("\n  ### | Sleep Voltage   | SW4:%1.3fV                               |", SW4_OUTPUT(sw4.sleep));
	printf("\n  ### |------------------------------------------------------------|");

	return HK_SUCCESS;
}
//------------------------------------------------

/*
 * date: 2016/10/12
 * modify : hk
 * description: Display pf0100 Boost Information
 */
u8 fnPF0100DisplayBoost(void)
{
	u8 	buf[8];
	u8  volt, modNormal, modStandby;

	//	Boost Regulator
	if(fnPMICRead(PF0100_I2C_BUS, PF0100_I2C_ADDRESS, REG_PF0100_SWBSTCTL, buf, 1) != HK_SUCCESS)
		return HK_FAIL;

	//printf("\n  ### hk_dbg, %s :addr:0x%02x, data:0x%02x!! \n", __FUNCTION__, REG_PF0100_SWBSTCTL, buf[0]);

	//	Voltage
	volt = REG_BST_VOL(buf[0]);
	modNormal = REG_BST_NORMAL(buf[0]);
	modStandby = REG_BST_STANDBY(buf[0]);

	printf("\n");
	printf("\n  ### |----------------------------------------------------------------------|");
	printf("\n  ### | BOOST Contorl List                                                   |");
	printf("\n  ### |----------------------------------------------------------------------|");
	printf("\n  ### | Output Port |                      Mode                  |  Voltage  |");
	printf("\n  ### |----------------------------------------------------------------------|");
	printf("\n  ### | BOOST       | Normal operation:%s & Standby:%s\t |   %1.3f   |", SWBST_MODE(modNormal), SWBST_MODE(modStandby), SWBST_OUTPUT(volt));
	printf("\n  ### |----------------------------------------------------------------------|");

	return HK_SUCCESS;
}
//------------------------------------------------

/*
 * date: 2016/10/12
 * modify : hk
 * description: Display pf0100 LDO Information
 */
u8 fnPF0100DisplayLDO(void)
{
	u8 volt[8];

	//	LDO
	{
		u8 i;

		for(i=0;i<6;i++)
		{
			if(fnPMICRead(PF0100_I2C_BUS, PF0100_I2C_ADDRESS, (REG_PF0100_VGEN1CTL + i), &volt[i], 1) != HK_SUCCESS)
				return HK_FAIL;

			//printf("\n  ### hk_dbg, %s :addr:0x%02x, data:0x%02x!! \n", __FUNCTION__, (REG_PF0100_VGEN1CTL + i), volt[i]);
		}
	}

	// LDO(VGEN1 ~ VGEN6)
	printf("\n");
	printf("\n  ### |--------------------------------------|");
	printf("\n  ### | LDO Contorl List                     |");
	printf("\n  ### |--------------------------------------|");
	printf("\n  ### | Output Port | Voltage  | Status      |");
	printf("\n  ### |--------------------------------------|");
	printf("\n  ### | VGEN1       | 0x%1.3f  | %s\t     |", VGEN12_OUTPUT(REG_VGENx_VOLT(volt[0])), VGENx_EN(REG_VGENx_EN(volt[0])));
	printf("\n  ### |--------------------------------------|");
	printf("\n  ### | VGEN2       | 0x%1.3f  | %s\t     |", VGEN12_OUTPUT(REG_VGENx_VOLT(volt[1])), VGENx_EN(REG_VGENx_EN(volt[1])));
	printf("\n  ### |--------------------------------------|");
	printf("\n  ### | VGEN3       | 0x%1.3f  | %s\t     |", VGEN3456_OUTPUT(REG_VGENx_VOLT(volt[2])), VGENx_EN(REG_VGENx_EN(volt[2])));
	printf("\n  ### |--------------------------------------|");
	printf("\n  ### | VGEN4       | 0x%1.3f  | %s\t     |", VGEN3456_OUTPUT(REG_VGENx_VOLT(volt[3])), VGENx_EN(REG_VGENx_EN(volt[3])));
	printf("\n  ### |--------------------------------------|");
	printf("\n  ### | VGEN5       | 0x%1.3f  | %s\t     |", VGEN3456_OUTPUT(REG_VGENx_VOLT(volt[4])), VGENx_EN(REG_VGENx_EN(volt[4])));
	printf("\n  ### |--------------------------------------|");
	printf("\n  ### | VGEN6       | 0x%1.3f  | %s\t     |", VGEN3456_OUTPUT(REG_VGENx_VOLT(volt[5])), VGENx_EN(REG_VGENx_EN(volt[5])));
	printf("\n  ### |--------------------------------------|");

	return HK_SUCCESS;
}
//------------------------------------------------

/*
 * date: 2016/10/12
 * modify : hk
 * description: Display pf0100 VSNVS Information
 */
u8 fnPF0100DisplayVSNVS(void)
{
	u8 volt[8];

	//	LDO
	if(fnPMICRead(PF0100_I2C_BUS, PF0100_I2C_ADDRESS, REG_PF0600_VSNVSCTL, volt, 1) != HK_SUCCESS)
		return HK_FAIL;

	//printf("\n  ### hk_dbg, %s :addr:0x%02x, data:0x%02x!! \n", __FUNCTION__, REG_PF0600_VSNVSCTL, volt[0]);

	// VSNVS
	printf("\n");
	printf("\n  ### |-------------------------|");
	printf("\n  ### | SNVS Contorl List       |");
	printf("\n  ### |-------------------------|");
	printf("\n  ### | Output Port |  Voltage  |");
	printf("\n  ### |-------------------------|");
	printf("\n  ### | VSNVS       |  0x%1.3f  |", VSNVS_OUTPUT(volt[0]));
	printf("\n  ### |-------------------------|");

	return HK_SUCCESS;
}
//------------------------------------------------

/*
 * date: 2016/10/13
 * modify : hk
 * description: Config pf0100 SW1234
 */
u8 fnPF0100ConfigBuck(void)
{
	u8 port;
	u8 point;

	//	Buck Port Selection
	printf("\n  Please Input LDO Port (1 ~ 4): ");

	port = fnSystemInputString();

    hk_dbg("\n  hk_dbg, %s : port::%d\n", __FUNCTION__, port);

    if((port < BUCK_PORT_1)||(port > BUCK_PORT_4))
    {
    	printf("\n  Out of Range, Please Try Again!! \n\n");

    	return HK_FAIL;
    }

    if(port == BUCK_PORT_1)
    {
		printf("\n");
		printf("\n  ### |-----------------------------------------------------------------------------|");
		printf("\n  ### |                      SW1A/B/C Output Voltage Configuration                  |");
		printf("\n  ### |--------------------------------------|--------------------------------------|");
		printf("\n  ### |   Input Number   |      Voltage      |   Input Number   |      Voltage      |");
		printf("\n  ### |--------------------------------------|--------------------------------------|");
		printf("\n  ### |         0        |       0.3000      |         32       |       1.1000      |");
		printf("\n  ### |         1        |       0.3250      |         33       |       1.1250      |");
		printf("\n  ### |         2        |       0.3500      |         34       |       1.1500      |");
		printf("\n  ### |         3        |       0.3750      |         35       |       1.1750      |");
		printf("\n  ### |         4        |       0.4000      |         36       |       1.2000      |");
		printf("\n  ### |         5        |       0.4250      |         37       |       1.2250      |");
		printf("\n  ### |         6        |       0.4500      |         38       |       1.2500      |");
		printf("\n  ### |         7        |       0.4750      |         39       |       1.2750      |");
		printf("\n  ### |         8        |       0.5000      |         40       |       1.3000      |");
		printf("\n  ### |         9        |       0.5250      |         41       |       1.3250      |");
		printf("\n  ### |         10       |       0.5500      |         42       |       1.3500      |");
		printf("\n  ### |         11       |       0.5750      |         43       |       1.3750      |");
		printf("\n  ### |         12       |       0.6000      |         44       |       1.4000      |");
		printf("\n  ### |         13       |       0.6250      |         45       |       1.4250      |");
		printf("\n  ### |         14       |       0.6500      |         46       |       1.4500      |");
		printf("\n  ### |         15       |       0.6750      |         47       |       1.4750      |");
		printf("\n  ### |         16       |       0.7000      |         48       |       1.5000      |");
		printf("\n  ### |         17       |       0.7250      |         49       |       1.5250      |");
		printf("\n  ### |         18       |       0.7500      |         50       |       1.5500      |");
		printf("\n  ### |         19       |       0.7750      |         51       |       1.5750      |");
		printf("\n  ### |         20       |       0.8000      |         52       |       1.6000      |");
		printf("\n  ### |         21       |       0.8250      |         53       |       1.6250      |");
		printf("\n  ### |         22       |       0.8500      |         54       |       1.6500      |");
		printf("\n  ### |         23       |       0.8750      |         55       |       1.6750      |");
		printf("\n  ### |         24       |       0.9000      |         56       |       1.7000      |");
		printf("\n  ### |         25       |       0.9250      |         57       |       1.7250      |");
		printf("\n  ### |         26       |       0.9500      |         58       |       1.7500      |");
		printf("\n  ### |         27       |       0.9750      |         59       |       1.7750      |");
		printf("\n  ### |         28       |       1.0000      |         60       |       1.8000      |");
		printf("\n  ### |         29       |       1.0250      |         61       |       1.8250      |");
		printf("\n  ### |         30       |       1.0500      |         62       |       1.8500      |");
        printf("\n  ### |         31       |       1.0750      |         63       |       1.8750      |");
		printf("\n  ### |-----------------------------------------------------------------------------|");
		printf("\n");

		printf("\n  Please Select Output Voltage (0 ~ 63): ");

		point = fnSystemInputString();

		//hk_dbg("\n  hk_dbg, %s : point:%d\n", __FUNCTION__, point);

		if(point > BUCK1_SETPOINT_63)
    	{
    		printf("\n  Out of Range, Please Try Again!! \n\n");

    		return HK_FAIL;
    	}

    	{
    		u8 data[8];

    		//	Read Register
			if(fnPMICRead(PF0100_I2C_BUS, PF0100_I2C_ADDRESS, REG_PF0100_SW1ABVOLT, data, 1) != HK_SUCCESS)
				return HK_FAIL;

			//printf("\n  ### hk_dbg, %s : reg:0x%02x, data:0x%02x\n", __FUNCTION__, REG_PF0100_SW1ABVOLT, data[0]);

			//	Mask Data
			data[0] = data[0] & 0xC0;

			data[0] = data[0] | point;

			//printf("\n  ### hk_dbg, %s : reg:0x%02x, data:0x%02x\n", __FUNCTION__, REG_PF0100_SW1ABVOLT, data[0]);

			//	Write Register
			if(fnPMICWrite(PF0100_I2C_BUS, PF0100_I2C_ADDRESS, REG_PF0100_SW1ABVOLT, data, 1) != HK_SUCCESS)
				return HK_FAIL;

			//	Display Buck 1 Output Voltage
			fnPF0100DisplaySW1ABC();
    	}
    }
    else if(port == BUCK_PORT_2)
    {
		printf("\n");
		printf("\n  ### |-----------------------------------------------------------------------------|");
		printf("\n  ### |                         SW2 Output Voltage Configuration                    |");
		printf("\n  ### |--------------------------------------|--------------------------------------|");
		printf("\n  ### |   Input Number   |      Voltage      |   Input Number   |      Voltage      |");
		printf("\n  ### |--------------------------------------|--------------------------------------|");
		printf("\n  ### |         0        |       0.4000      |         64       |       0.8000      |");
		printf("\n  ### |         1        |       0.4250      |         65       |       0.8500      |");
		printf("\n  ### |         2        |       0.4500      |         66       |       0.9000      |");
		printf("\n  ### |         3        |       0.4750      |         67       |       0.9500      |");
		printf("\n  ### |         4        |       0.5000      |         68       |       1.0000      |");
		printf("\n  ### |         5        |       0.5250      |         69       |       1.0500      |");
		printf("\n  ### |         6        |       0.5500      |         60       |       1.1000      |");
		printf("\n  ### |         7        |       0.5750      |         71       |       1.1500      |");
		printf("\n  ### |         8        |       0.6000      |         72       |       1.2000      |");
		printf("\n  ### |         9        |       0.6250      |         73       |       1.2500      |");
		printf("\n  ### |         10       |       0.6500      |         74       |       1.3000      |");
		printf("\n  ### |         11       |       0.6750      |         75       |       1.3500      |");
		printf("\n  ### |         12       |       0.7000      |         76       |       1.4000      |");
		printf("\n  ### |         13       |       0.7250      |         77       |       1.4500      |");
		printf("\n  ### |         14       |       0.7500      |         78       |       1.5000      |");
		printf("\n  ### |         15       |       0.7750      |         79       |       1.5500      |");
		printf("\n  ### |         16       |       0.8000      |         80       |       1.6000      |");
		printf("\n  ### |         17       |       0.8250      |         81       |       1.6500      |");
		printf("\n  ### |         18       |       0.8500      |         82       |       1.7000      |");
		printf("\n  ### |         19       |       0.8750      |         83       |       1.7500      |");
		printf("\n  ### |         20       |       0.9000      |         84       |       1.8000      |");
		printf("\n  ### |         21       |       0.9250      |         85       |       1.8500      |");
		printf("\n  ### |         22       |       0.9500      |         86       |       1.9000      |");
		printf("\n  ### |         23       |       0.9750      |         87       |       1.9500      |");
		printf("\n  ### |         24       |       1.0000      |         88       |       2.0000      |");
		printf("\n  ### |         25       |       1.0250      |         89       |       2.0500      |");
		printf("\n  ### |         26       |       1.0500      |         90       |       2.1000      |");
		printf("\n  ### |         27       |       1.0750      |         91       |       2.1500      |");
		printf("\n  ### |         28       |       1.1000      |         92       |       2.2000      |");
		printf("\n  ### |         29       |       1.1250      |         93       |       2.2500      |");
		printf("\n  ### |         30       |       1.1500      |         94       |       2.3000      |");
        printf("\n  ### |         31       |       1.1750      |         95       |       2.3500      |");
		printf("\n  ### |         32       |       1.2000      |         96       |       2.4000      |");
	    printf("\n  ### |         33       |       1.2250      |         97       |       2.4500      |");
        printf("\n  ### |         34       |       1.2500      |         98       |       2.5000      |");
        printf("\n  ### |         35       |       1.2750      |         99       |       2.5500      |");
        printf("\n  ### |         36       |       1.3000      |         100      |       2.6000      |");
        printf("\n  ### |         37       |       1.3250      |         101      |       2.6500      |");
        printf("\n  ### |         38       |       1.3500      |         102      |       2.7000      |");
        printf("\n  ### |         39       |       1.3750      |         103      |       2.7500      |");
        printf("\n  ### |         40       |       1.4000      |         104      |       2.8000      |");
        printf("\n  ### |         41       |       1.4250      |         105      |       2.8500      |");
        printf("\n  ### |         42       |       1.4500      |         106      |       2.9000      |");
        printf("\n  ### |         43       |       1.4750      |         107      |       2.9500      |");
        printf("\n  ### |         44       |       1.5000      |         108      |       3.0000      |");
        printf("\n  ### |         45       |       1.5250      |         109      |       3.0500      |");
        printf("\n  ### |         46       |       1.5500      |         110      |       3.1000      |");
        printf("\n  ### |         47       |       1.5750      |         111      |       3.1500      |");
        printf("\n  ### |         48       |       1.6000      |         112      |       3.2000      |");
        printf("\n  ### |         49       |       1.6250      |         113      |       3.2500      |");
        printf("\n  ### |         50       |       1.6500      |         114      |       Reserved    |");
        printf("\n  ### |         51       |       1.6750      |         115      |       Reserved    |");
        printf("\n  ### |         52       |       1.7000      |         116      |       Reserved    |");
        printf("\n  ### |         53       |       1.7250      |         117      |       Reserved    |");
        printf("\n  ### |         54       |       1.7500      |         118      |       Reserved    |");
        printf("\n  ### |         55       |       1.7750      |         119      |       Reserved    |");
        printf("\n  ### |         56       |       1.8000      |         120      |       Reserved    |");
        printf("\n  ### |         57       |       1.8250      |         121      |       Reserved    |");
        printf("\n  ### |         58       |       1.8500      |         122      |       Reserved    |");
        printf("\n  ### |         59       |       1.8750      |         123      |       Reserved    |");
        printf("\n  ### |         60       |       1.9000      |         124      |       Reserved    |");
        printf("\n  ### |         61       |       1.9250      |         125      |       Reserved    |");
        printf("\n  ### |         62       |       1.9500      |         126      |       Reserved    |");
        printf("\n  ### |         63       |       1.9750      |         127      |       Reserved    |");
		printf("\n  ### |--------------------------------------|--------------------------------------|");

		printf("\n  Please Select Output Voltage (0 ~ 114): ");

		point = fnSystemInputString();

		//hk_dbg("\n  hk_dbg, %s : point:%d\n", __FUNCTION__, point);

		if(point > BUCK2_SETPOINT_114)
    	{
    		printf("\n  Out of Range, Please Try Again!! \n\n");

    		return HK_FAIL;
    	}

    	{
    		u8 data[8];

    		//	Read Register
			if(fnPMICRead(PF0100_I2C_BUS, PF0100_I2C_ADDRESS, REG_PF0100_SW2VOLT, data, 1) != HK_SUCCESS)
				return HK_FAIL;

			//printf("\n  ### hk_dbg, %s : reg:0x%02x, data:0x%02x\n", __FUNCTION__, REG_PF0100_SW2VOLT, data[0]);

			//	Mask Data
			data[0] = data[0] & 0x80;

			data[0] = data[0] | point;

			//printf("\n  ### hk_dbg, %s : reg:0x%02x, data:0x%02x\n", __FUNCTION__, REG_PF0100_SW2VOLT, data[0]);

			//	Write Register
			if(fnPMICWrite(PF0100_I2C_BUS, PF0100_I2C_ADDRESS, REG_PF0100_SW2VOLT, data, 1) != HK_SUCCESS)
				return HK_FAIL;

			//	Display Buck 2 Output Voltage
			fnPF0100DisplaySW2();
    	}
    }
	else if(port == BUCK_PORT_3)
    {
		printf("\n");
		printf("\n  ### |-----------------------------------------------------------------------------|");
		printf("\n  ### |                        SW3A/B Output Voltage Configuration                  |");
		printf("\n  ### |--------------------------------------|--------------------------------------|");
		printf("\n  ### |   Input Number   |      Voltage      |   Input Number   |      Voltage      |");
		printf("\n  ### |--------------------------------------|--------------------------------------|");
		printf("\n  ### |         0        |       0.4000      |         64       |       0.8000      |");
		printf("\n  ### |         1        |       0.4250      |         65       |       0.8500      |");
		printf("\n  ### |         2        |       0.4500      |         66       |       0.9000      |");
		printf("\n  ### |         3        |       0.4750      |         67       |       0.9500      |");
		printf("\n  ### |         4        |       0.5000      |         68       |       1.0000      |");
		printf("\n  ### |         5        |       0.5250      |         69       |       1.0500      |");
		printf("\n  ### |         6        |       0.5500      |         60       |       1.1000      |");
		printf("\n  ### |         7        |       0.5750      |         71       |       1.1500      |");
		printf("\n  ### |         8        |       0.6000      |         72       |       1.2000      |");
		printf("\n  ### |         9        |       0.6250      |         73       |       1.2500      |");
		printf("\n  ### |         10       |       0.6500      |         74       |       1.3000      |");
		printf("\n  ### |         11       |       0.6750      |         75       |       1.3500      |");
		printf("\n  ### |         12       |       0.7000      |         76       |       1.4000      |");
		printf("\n  ### |         13       |       0.7250      |         77       |       1.4500      |");
		printf("\n  ### |         14       |       0.7500      |         78       |       1.5000      |");
		printf("\n  ### |         15       |       0.7750      |         79       |       1.5500      |");
		printf("\n  ### |         16       |       0.8000      |         80       |       1.6000      |");
		printf("\n  ### |         17       |       0.8250      |         81       |       1.6500      |");
		printf("\n  ### |         18       |       0.8500      |         82       |       1.7000      |");
		printf("\n  ### |         19       |       0.8750      |         83       |       1.7500      |");
		printf("\n  ### |         20       |       0.9000      |         84       |       1.8000      |");
		printf("\n  ### |         21       |       0.9250      |         85       |       1.8500      |");
		printf("\n  ### |         22       |       0.9500      |         86       |       1.9000      |");
		printf("\n  ### |         23       |       0.9750      |         87       |       1.9500      |");
		printf("\n  ### |         24       |       1.0000      |         88       |       2.0000      |");
		printf("\n  ### |         25       |       1.0250      |         89       |       2.0500      |");
		printf("\n  ### |         26       |       1.0500      |         90       |       2.1000      |");
		printf("\n  ### |         27       |       1.0750      |         91       |       2.1500      |");
		printf("\n  ### |         28       |       1.1000      |         92       |       2.2000      |");
		printf("\n  ### |         29       |       1.1250      |         93       |       2.2500      |");
		printf("\n  ### |         30       |       1.1500      |         94       |       2.3000      |");
        printf("\n  ### |         31       |       1.1750      |         95       |       2.3500      |");
		printf("\n  ### |         32       |       1.2000      |         96       |       2.4000      |");
	    printf("\n  ### |         33       |       1.2250      |         97       |       2.4500      |");
        printf("\n  ### |         34       |       1.2500      |         98       |       2.5000      |");
        printf("\n  ### |         35       |       1.2750      |         99       |       2.5500      |");
        printf("\n  ### |         36       |       1.3000      |         100      |       2.6000      |");
        printf("\n  ### |         37       |       1.3250      |         101      |       2.6500      |");
        printf("\n  ### |         38       |       1.3500      |         102      |       2.7000      |");
        printf("\n  ### |         39       |       1.3750      |         103      |       2.7500      |");
        printf("\n  ### |         40       |       1.4000      |         104      |       2.8000      |");
        printf("\n  ### |         41       |       1.4250      |         105      |       2.8500      |");
        printf("\n  ### |         42       |       1.4500      |         106      |       2.9000      |");
        printf("\n  ### |         43       |       1.4750      |         107      |       2.9500      |");
        printf("\n  ### |         44       |       1.5000      |         108      |       3.0000      |");
        printf("\n  ### |         45       |       1.5250      |         109      |       3.0500      |");
        printf("\n  ### |         46       |       1.5500      |         110      |       3.1000      |");
        printf("\n  ### |         47       |       1.5750      |         111      |       3.1500      |");
        printf("\n  ### |         48       |       1.6000      |         112      |       3.2000      |");
        printf("\n  ### |         49       |       1.6250      |         113      |       3.2500      |");
        printf("\n  ### |         50       |       1.6500      |         114      |       Reserved    |");
        printf("\n  ### |         51       |       1.6750      |         115      |       Reserved    |");
        printf("\n  ### |         52       |       1.7000      |         116      |       Reserved    |");
        printf("\n  ### |         53       |       1.7250      |         117      |       Reserved    |");
        printf("\n  ### |         54       |       1.7500      |         118      |       Reserved    |");
        printf("\n  ### |         55       |       1.7750      |         119      |       Reserved    |");
        printf("\n  ### |         56       |       1.8000      |         120      |       Reserved    |");
        printf("\n  ### |         57       |       1.8250      |         121      |       Reserved    |");
        printf("\n  ### |         58       |       1.8500      |         122      |       Reserved    |");
        printf("\n  ### |         59       |       1.8750      |         123      |       Reserved    |");
        printf("\n  ### |         60       |       1.9000      |         124      |       Reserved    |");
        printf("\n  ### |         61       |       1.9250      |         125      |       Reserved    |");
        printf("\n  ### |         62       |       1.9500      |         126      |       Reserved    |");
        printf("\n  ### |         63       |       1.9750      |         127      |       Reserved    |");
		printf("\n  ### |--------------------------------------|--------------------------------------|");

		printf("\n  Please Select Output Voltage (0 ~ 114): ");

		point = fnSystemInputString();

		//hk_dbg("\n  hk_dbg, %s : point:%d\n", __FUNCTION__, point);

		if(point > BUCK3_SETPOINT_114)
    	{
    		printf("\n  Out of Range, Please Try Again!! \n\n");

    		return HK_FAIL;
    	}

    	{
    		u8 data[8];

    		//	Read Register
			if(fnPMICRead(PF0100_I2C_BUS, PF0100_I2C_ADDRESS, REG_PF0100_SW3AVOLT, data, 1) != HK_SUCCESS)
				return HK_FAIL;

			//printf("\n  ### hk_dbg, %s : reg:0x%02x, data:0x%02x\n", __FUNCTION__, REG_PF0100_SW2VOLT, data[0]);

			//	Mask Data
			data[0] = data[0] & 0x80;

			data[0] = data[0] | point;

			//printf("\n  ### hk_dbg, %s : reg:0x%02x, data:0x%02x\n", __FUNCTION__, REG_PF0100_SW2VOLT, data[0]);

			//	Write Register
			if(fnPMICWrite(PF0100_I2C_BUS, PF0100_I2C_ADDRESS, REG_PF0100_SW3AVOLT, data, 1) != HK_SUCCESS)
				return HK_FAIL;

			//	Display Buck 3 Output Voltage
			fnPF0100DisplaySW3AB();
    	}
    }
	else if(port == BUCK_PORT_4)
    {
		printf("\n");
		printf("\n  ### |-----------------------------------------------------------------------------|");
		printf("\n  ### |                          SW4 Output Voltage Configuration                   |");
		printf("\n  ### |--------------------------------------|--------------------------------------|");
		printf("\n  ### |   Input Number   |      Voltage      |   Input Number   |      Voltage      |");
		printf("\n  ### |--------------------------------------|--------------------------------------|");
		printf("\n  ### |         0        |       0.4000      |         64       |       0.8000      |");
		printf("\n  ### |         1        |       0.4250      |         65       |       0.8500      |");
		printf("\n  ### |         2        |       0.4500      |         66       |       0.9000      |");
		printf("\n  ### |         3        |       0.4750      |         67       |       0.9500      |");
		printf("\n  ### |         4        |       0.5000      |         68       |       1.0000      |");
		printf("\n  ### |         5        |       0.5250      |         69       |       1.0500      |");
		printf("\n  ### |         6        |       0.5500      |         60       |       1.1000      |");
		printf("\n  ### |         7        |       0.5750      |         71       |       1.1500      |");
		printf("\n  ### |         8        |       0.6000      |         72       |       1.2000      |");
		printf("\n  ### |         9        |       0.6250      |         73       |       1.2500      |");
		printf("\n  ### |         10       |       0.6500      |         74       |       1.3000      |");
		printf("\n  ### |         11       |       0.6750      |         75       |       1.3500      |");
		printf("\n  ### |         12       |       0.7000      |         76       |       1.4000      |");
		printf("\n  ### |         13       |       0.7250      |         77       |       1.4500      |");
		printf("\n  ### |         14       |       0.7500      |         78       |       1.5000      |");
		printf("\n  ### |         15       |       0.7750      |         79       |       1.5500      |");
		printf("\n  ### |         16       |       0.8000      |         80       |       1.6000      |");
		printf("\n  ### |         17       |       0.8250      |         81       |       1.6500      |");
		printf("\n  ### |         18       |       0.8500      |         82       |       1.7000      |");
		printf("\n  ### |         19       |       0.8750      |         83       |       1.7500      |");
		printf("\n  ### |         20       |       0.9000      |         84       |       1.8000      |");
		printf("\n  ### |         21       |       0.9250      |         85       |       1.8500      |");
		printf("\n  ### |         22       |       0.9500      |         86       |       1.9000      |");
		printf("\n  ### |         23       |       0.9750      |         87       |       1.9500      |");
		printf("\n  ### |         24       |       1.0000      |         88       |       2.0000      |");
		printf("\n  ### |         25       |       1.0250      |         89       |       2.0500      |");
		printf("\n  ### |         26       |       1.0500      |         90       |       2.1000      |");
		printf("\n  ### |         27       |       1.0750      |         91       |       2.1500      |");
		printf("\n  ### |         28       |       1.1000      |         92       |       2.2000      |");
		printf("\n  ### |         29       |       1.1250      |         93       |       2.2500      |");
		printf("\n  ### |         30       |       1.1500      |         94       |       2.3000      |");
        printf("\n  ### |         31       |       1.1750      |         95       |       2.3500      |");
		printf("\n  ### |         32       |       1.2000      |         96       |       2.4000      |");
	    printf("\n  ### |         33       |       1.2250      |         97       |       2.4500      |");
        printf("\n  ### |         34       |       1.2500      |         98       |       2.5000      |");
        printf("\n  ### |         35       |       1.2750      |         99       |       2.5500      |");
        printf("\n  ### |         36       |       1.3000      |         100      |       2.6000      |");
        printf("\n  ### |         37       |       1.3250      |         101      |       2.6500      |");
        printf("\n  ### |         38       |       1.3500      |         102      |       2.7000      |");
        printf("\n  ### |         39       |       1.3750      |         103      |       2.7500      |");
        printf("\n  ### |         40       |       1.4000      |         104      |       2.8000      |");
        printf("\n  ### |         41       |       1.4250      |         105      |       2.8500      |");
        printf("\n  ### |         42       |       1.4500      |         106      |       2.9000      |");
        printf("\n  ### |         43       |       1.4750      |         107      |       2.9500      |");
        printf("\n  ### |         44       |       1.5000      |         108      |       3.0000      |");
        printf("\n  ### |         45       |       1.5250      |         109      |       3.0500      |");
        printf("\n  ### |         46       |       1.5500      |         110      |       3.1000      |");
        printf("\n  ### |         47       |       1.5750      |         111      |       3.1500      |");
        printf("\n  ### |         48       |       1.6000      |         112      |       3.2000      |");
        printf("\n  ### |         49       |       1.6250      |         113      |       3.2500      |");
        printf("\n  ### |         50       |       1.6500      |         114      |       3.3000      |");
        printf("\n  ### |         51       |       1.6750      |         115      |       Reserved    |");
        printf("\n  ### |         52       |       1.7000      |         116      |       Reserved    |");
        printf("\n  ### |         53       |       1.7250      |         117      |       Reserved    |");
        printf("\n  ### |         54       |       1.7500      |         118      |       Reserved    |");
        printf("\n  ### |         55       |       1.7750      |         119      |       Reserved    |");
        printf("\n  ### |         56       |       1.8000      |         120      |       Reserved    |");
        printf("\n  ### |         57       |       1.8250      |         121      |       Reserved    |");
        printf("\n  ### |         58       |       1.8500      |         122      |       Reserved    |");
        printf("\n  ### |         59       |       1.8750      |         123      |       Reserved    |");
        printf("\n  ### |         60       |       1.9000      |         124      |       Reserved    |");
        printf("\n  ### |         61       |       1.9250      |         125      |       Reserved    |");
        printf("\n  ### |         62       |       1.9500      |         126      |       Reserved    |");
        printf("\n  ### |         63       |       1.9750      |         127      |       Reserved    |");
		printf("\n  ### |--------------------------------------|--------------------------------------|");

		printf("\n  Please Select Output Voltage (0 ~ 114): ");

		point = fnSystemInputString();

		//hk_dbg("\n  hk_dbg, %s : point:%d\n", __FUNCTION__, point);

		if(point > BUCK4_SETPOINT_114)
    	{
    		printf("\n  Out of Range, Please Try Again!! \n\n");

    		return HK_FAIL;
    	}

    	{
    		u8 data[8];

    		//	Read Register
			if(fnPMICRead(PF0100_I2C_BUS, PF0100_I2C_ADDRESS, REG_PF0100_SW4VOLT, data, 1) != HK_SUCCESS)
				return HK_FAIL;

			//printf("\n  ### hk_dbg, %s : reg:0x%02x, data:0x%02x\n", __FUNCTION__, REG_PF0100_SW4VOLT, data[0]);

			//	Mask Data
			data[0] = data[0] & 0x80;

			data[0] = data[0] | point;

			//printf("\n  ### hk_dbg, %s : reg:0x%02x, data:0x%02x\n", __FUNCTION__, REG_PF0100_SW4VOLT, data[0]);

			//	Write Register
			if(fnPMICWrite(PF0100_I2C_BUS, PF0100_I2C_ADDRESS, REG_PF0100_SW4VOLT, data, 1) != HK_SUCCESS)
				return HK_FAIL;

			//	Display Buck 4 Output Voltage
			fnPF0100DisplaySW4();
    	}
    }

	return HK_SUCCESS;
}
//------------------------------------------------

/*
 * date: 2016/10/13
 * modify : hk
 * description: Config pf0100 Boost
 */
u8 fnPF0100ConfigBoost(void)
{
	u8 normal;
	u8 standby;
	u8 point;

	//	Output Voltage
	printf("\n");
		printf("\n  ### |--------------------------------------|");
		printf("\n  ### |     Output Voltage Configuration     |");
		printf("\n  ### |--------------------------------------|");
		printf("\n  ### |   Input Number   |      Voltage      |");
		printf("\n  ### |--------------------------------------|");
		printf("\n  ### |         0        |       5.000       |");
		printf("\n  ### |         1        |       5.050       |");
		printf("\n  ### |         2        |       5.100       |");
		printf("\n  ### |         3        |       5.150       |");
		printf("\n  ### |--------------------------------------|");
		printf("\n");

	printf("\n  Please Select Output Voltage (0 ~ 3): ");

	point = fnSystemInputString();

	//hk_dbg("\n  hk_dbg, %s : point:%d\n", __FUNCTION__, point);

	if(point > BST_SETPOINT_3)
    {
    	printf("\n  Out of Range, Please Try Again!! \n\n");

    	return HK_FAIL;
    }

    //	Normal Operation Switch Mode
    printf("\n");
		printf("\n  ### |--------------------------------------|");
		printf("\n  ### |     Normal Operation Switch Mode     |");
		printf("\n  ### |--------------------------------------|");
		printf("\n  ### |   Input Number   |      Voltage      |");
		printf("\n  ### |--------------------------------------|");
		printf("\n  ### |         0        |        OFF        |");
		printf("\n  ### |         1        |        PFM        |");
		printf("\n  ### |         2        |        Auto       |");
		printf("\n  ### |         3        |        APS        |");
		printf("\n  ### |--------------------------------------|");
		printf("\n");

	printf("\n  Please Select Normal Operation Switch Mode (0 ~ 3): ");

	normal = fnSystemInputString();

	//hk_dbg("\n  hk_dbg, %s : normal:%d\n", __FUNCTION__, normal);

	if(normal > BOOST_SWITCH_3)
    {
    	printf("\n  Out of Range, Please Try Again!! \n\n");

    	return HK_FAIL;
    }

    //	Standby Switch Mode
    printf("\n");
		printf("\n  ### |--------------------------------------|");
		printf("\n  ### |          Standby Switch Mode         |");
		printf("\n  ### |--------------------------------------|");
		printf("\n  ### |   Input Number   |      Voltage      |");
		printf("\n  ### |--------------------------------------|");
		printf("\n  ### |         0        |        OFF        |");
		printf("\n  ### |         1        |        PFM        |");
		printf("\n  ### |         2        |        Auto       |");
		printf("\n  ### |         3        |        APS        |");
		printf("\n  ### |--------------------------------------|");
		printf("\n");

	printf("\n  Please Select Standby Switch Mode (0 ~ 3): ");

	standby = fnSystemInputString();

	//hk_dbg("\n  hk_dbg, %s : standby:%d\n", __FUNCTION__, standby);

	if(standby > BOOST_SWITCH_3)
    {
    	printf("\n  Out of Range, Please Try Again!! \n\n");

    	return HK_FAIL;
    }

    {
    	u8 data[8];

    	//	Read Register
		if(fnPMICRead(PF0100_I2C_BUS, PF0100_I2C_ADDRESS, REG_PF0100_SWBSTCTL, data, 1) != HK_SUCCESS)
			return HK_FAIL;

		//printf("\n  ### hk_dbg, %s : reg:0x%02x, data:0x%02x\n", __FUNCTION__, REG_PF0100_SWBSTCTL, data[0]);

		data[0] = (standby <<5) | (normal<<2) | point;

		//printf("\n  ### hk_dbg, %s : reg:0x%02x, data:0x%02x\n", __FUNCTION__, REG_PF0100_SWBSTCTL, data[0]);

		//	Write Register
		if(fnPMICWrite(PF0100_I2C_BUS, PF0100_I2C_ADDRESS, REG_PF0100_SWBSTCTL, data, 1) != HK_SUCCESS)
			return HK_FAIL;

		//	Display Boost Output Voltage
		fnPF0100DisplayBoost();
    }

	return HK_SUCCESS;
}
//------------------------------------------------

/*
 * date: 2016/10/13
 * modify : hk
 * description: Config pf0100 SNVS
 */
u8 fnPF0100ConfigLDO(void)
{
	#define LDO_PORT(x)	\
						((x == 1)?REG_PF0100_VGEN1CTL:	\
						((x == 2)?REG_PF0100_VGEN2CTL:	\
						((x == 3)?REG_PF0100_VGEN3CTL:	\
						((x == 4)?REG_PF0100_VGEN4CTL:	\
						((x == 5)?REG_PF0100_VGEN5CTL:	\
						((x == 6)?REG_PF0100_VGEN6CTL: 0xFF))))))	// 	LDO Port

	u8 port;
	u8 ctrl;
	u8 point;

	//	LDO Port Selection
	printf("\n  Please Input LDO Port (1 ~ 6): ");

	port = fnSystemInputString();

    //hk_dbg("\n  hk_dbg, %s : port::%d\n", __FUNCTION__, port);

    if((port < LDO_PORT_1)||(port > LDO_PORT_6))
    {
    	printf("\n  Out of Range, Please Try Again!! \n\n");

    	return HK_FAIL;
    }

	//	LDO Port Control
    printf("\n  Please Select Control Mode(0:Disable, 1:Enable): ");

	ctrl = fnSystemInputString();

	//hk_dbg("\n  hk_dbg, %s : ctrl::%d\n", __FUNCTION__, ctrl);

	if(ctrl > LDO_ENABLE)
    {
    	printf("\n  Out of Range, Please Try Again!! \n\n");

    	return HK_FAIL;
    }

    //	LDO Output Voltage
    if(port < LDO_PORT_3)
    {
    	printf("\n");
		printf("\n  ### |--------------------------------------|");
		printf("\n  ### |     Output Voltage Configuration     |");
		printf("\n  ### |--------------------------------------|");
		printf("\n  ### |   Input Number   |      Voltage      |");
		printf("\n  ### |--------------------------------------|");
		printf("\n  ### |         0        |       0.800       |");
		printf("\n  ### |         1        |       0.850       |");
		printf("\n  ### |         2        |       0.900       |");
		printf("\n  ### |         3        |       0.950       |");
		printf("\n  ### |         4        |       1.000       |");
		printf("\n  ### |         5        |       1.050       |");
		printf("\n  ### |         6        |       1.100       |");
		printf("\n  ### |         7        |       1.150       |");
		printf("\n  ### |         8        |       1.200       |");
		printf("\n  ### |         9        |       1.250       |");
		printf("\n  ### |         10       |       1.300       |");
		printf("\n  ### |         11       |       1.350       |");
		printf("\n  ### |         12       |       1.400       |");
		printf("\n  ### |         13       |       1.450       |");
		printf("\n  ### |         14       |       1.500       |");
		printf("\n  ### |         15       |       1.550       |");
		printf("\n  ### |--------------------------------------|");
		printf("\n");
    }
    else
	{
		printf("\n");
		printf("\n  ### |--------------------------------------|");
		printf("\n  ### |     Output Voltage Configuration     |");
		printf("\n  ### |--------------------------------------|");
		printf("\n  ### |   Input Number   |      Voltage      |");
		printf("\n  ### |--------------------------------------|");
		printf("\n  ### |         0        |       1.80        |");
		printf("\n  ### |         1        |       1.90        |");
		printf("\n  ### |         2        |       2.00        |");
		printf("\n  ### |         3        |       2.10        |");
		printf("\n  ### |         4        |       2.20        |");
		printf("\n  ### |         5        |       2.30        |");
		printf("\n  ### |         6        |       2.40        |");
		printf("\n  ### |         7        |       2.50        |");
		printf("\n  ### |         8        |       2.60        |");
		printf("\n  ### |         9        |       2.70        |");
		printf("\n  ### |         10       |       2.80        |");
		printf("\n  ### |         11       |       2.90        |");
		printf("\n  ### |         12       |       3.00        |");
		printf("\n  ### |         13       |       3.10        |");
		printf("\n  ### |         14       |       3.20        |");
		printf("\n  ### |         15       |       3.30        |");
		printf("\n  ### |--------------------------------------|");
		printf("\n");
	}

	printf("\n  Please Select Output Voltage (0 ~ 15): ");

	point = fnSystemInputString();

	//hk_dbg("\n  hk_dbg, %s : point::%d\n", __FUNCTION__, point);

	if(ctrl> LDO_SETPOINT_15)
    {
    	printf("\n  Out of Range, Please Try Again!! \n\n");

    	return HK_FAIL;
    }

    {
    	u8 reg;
    	u8 data[8];

		//	Regiser
    	reg = LDO_PORT(port);

    	//	Read Register
		if(fnPMICRead(PF0100_I2C_BUS, PF0100_I2C_ADDRESS, reg, data, 1) != HK_SUCCESS)
			return HK_FAIL;

		//printf("\n  ### hk_dbg, %s : reg:0x%02x, data:0x%02x\n", __FUNCTION__, reg, data[0]);

    	//	Mask Output Voltage & Control
		data[0] = data[0] & 0xE0;

		data[0] = data[0] | (ctrl << 4) | point;

		//printf("\n  ### hk_dbg, %s : reg:0x%02x, data:0x%02x\n", __FUNCTION__, reg, data[0]);

		//	Write Register
		if(fnPMICWrite(PF0100_I2C_BUS, PF0100_I2C_ADDRESS, reg, data, 1) != HK_SUCCESS)
			return HK_FAIL;

		//	Display LDO Output Voltage
		fnPF0100DisplayLDO();
    }

	return HK_SUCCESS;
}
//------------------------------------------------

/*
 * date: 2016/10/13
 * modify : hk
 * description: Config pf0100 SNVS
 */
u8 fnPF0100ConfigSNVS(void)
{
	u8 point;

	printf("\n");
		printf("\n  ### |--------------------------------------|");
		printf("\n  ### |     Output Voltage Configuration     |");
		printf("\n  ### |--------------------------------------|");
		printf("\n  ### |   Input Number   |      Voltage      |");
		printf("\n  ### |--------------------------------------|");
		printf("\n  ### |         0        |       1.0         |");
		printf("\n  ### |         1        |       1.1         |");
		printf("\n  ### |         2        |       1.2         |");
		printf("\n  ### |         3        |       1.3         |");
		printf("\n  ### |         4        |       1.5         |");
		printf("\n  ### |         5        |       1.8         |");
		printf("\n  ### |         6        |       3.0         |");
		printf("\n  ### |--------------------------------------|");
		printf("\n");

	printf("\n  Please Select Output Voltage (0 ~ 6): ");

	point = fnSystemInputString();

	//hk_dbg("\n  hk_dbg, %s : point:%d\n", __FUNCTION__, point);

	if(point > SNVS_SETPOINT_6)
    {
    	printf("\n  Out of Range, Please Try Again!! \n\n");

    	return HK_FAIL;
    }

    {
    	u8 data[8];

    	//	Read Register
		if(fnPMICRead(PF0100_I2C_BUS, PF0100_I2C_ADDRESS, REG_PF0600_VSNVSCTL, data, 1) != HK_SUCCESS)
			return HK_FAIL;

		//printf("\n  ### hk_dbg, %s : reg:0x%02x, data:0x%02x\n", __FUNCTION__, REG_PF0600_VSNVSCTL, data[0]);

    	//	Mask Output Voltage & Control
		data[0] = data[0] & 0xF8;

		data[0] = data[0] | point;

		//printf("\n  ### hk_dbg, %s : reg:0x%02x, data:0x%02x\n", __FUNCTION__, REG_PF0600_VSNVSCTL, data[0]);

		//	Write Register
		if(fnPMICWrite(PF0100_I2C_BUS, PF0100_I2C_ADDRESS, REG_PF0600_VSNVSCTL, data, 1) != HK_SUCCESS)
			return HK_FAIL;

		//	Display SNVS Output Voltage
		fnPF0100DisplayVSNVS();
    }

	return HK_SUCCESS;
}
//------------------------------------------------

/*
 * date: 2016/10/05
 * modify : hk
 * description: Initial pf0100
 */
u8 fnPF0100Init(void)
{
	return HK_SUCCESS;
}
//------------------------------------------------

/*
 * date: 2016/10/11
 * modify : hk
 * description: List pf0100 Current Setting
 */
u8 fnPF0100List(void)
{
	// 	Display SW1ABC Information
	fnPF0100DisplaySW1ABC();

	//	Display SW2 Information
	fnPF0100DisplaySW2();

	//	Display SW3AB Information
	fnPF0100DisplaySW3AB();

	//	Display SW4 Information
	fnPF0100DisplaySW4();

	//	Display Boost Regulator
	fnPF0100DisplayBoost();

	//	Display LDO
	fnPF0100DisplayLDO();

	//	Display VSNVS
	fnPF0100DisplayVSNVS();

	return HK_SUCCESS;
}
//------------------------------------------------

/*
 * date: 2016/10/11
 * modify : hk
 * description: Config pf0100 Current Setting
 */
u8 fnPF0100Config(void)
{
	u8	idx;
	u32 cmd;

	while(1)
	{
		idx = 1;

		printf("\r\n  ========================  PMIC Config ====================== \r\n\n");
		printf("  %d. Buck									  		  			   \r\n\n", idx++);
		printf("  %d. Boost							  		  					   \r\n\n", idx++);
    	printf("  %d. LDO										  		  		   \r\n\n", idx++);
    	printf("  %d. SNVS											  			   \r\n\n", idx++);
    	printf("  %d. Return  													   \r\n\n", idx);
    	printf("  Please Enter Command(1 ~ %d): ", idx);

    	cmd = fnSystemInputString();

    	//hk_dbg("### hk_dbg, %s : cmd:%d\n", __FUNCTION__, cmd);

    	switch(cmd)
    	{
    		case PCF_BUCK:
    			fnPF0100ConfigBuck();
    		break;

    		case PCF_BOOST:
    			fnPF0100ConfigBoost();
    		break;

    		case PCF_LDO:
    			fnPF0100ConfigLDO();
    		break;

    		case PCF_SNVS:
				fnPF0100ConfigSNVS();
    		break;

			case PCF_RETURN:
				return HK_NA;
			break;

			default:
				hk_dbg("\n  ### hk_dbg, %s : Unknow command:%d, Please Try again! \n", __FUNCTION__, cmd);
			break;
    	}

    	sleep(1);
	}

	return HK_SUCCESS;
}
//------------------------------------------------

/*
 * date: 2016/10/11
 * modify : hk
 * description: Auto Test pf0100
 */
u8 fnPF0100Test(void)
{
	u8 	buf[32];

	//	Check Device ID
	if(fnPMICRead(PF0100_I2C_BUS, PF0100_I2C_ADDRESS, REG_PF0100_DEVID, buf, 1) != HK_SUCCESS)
		return HK_FAIL;

	//	Compare Device ID
	if(buf[0] != PF0100_ID)
	{
		printf("\n  ### hk_dbg, %s : error device id:0x%02x\n", __FUNCTION__, buf[0]);

		return HK_FAIL;
	}

	//	Buck Check
	{
		u8 i;

		u8 reg[8]={REG_PF0100_SW1ABVOLT, REG_PF0100_SW1CVOLT, REG_PF0100_SW2VOLT, REG_PF0100_SW3AVOLT, REG_PF0100_SW3BVOLT, REG_PF0100_SW4VOLT};
		u8 defData[8];
		u8 setData[8];
		u8 getData[8];

		for(i=0;i<6;i++)
		{
			//	Read Default Setting
			if(fnPMICRead(PF0100_I2C_BUS, PF0100_I2C_ADDRESS, reg[i], &defData[i], 1) != HK_SUCCESS)
				return HK_FAIL;

			//printf("### hk_dbg, %s : reg:0x%02x, data:0x%2x!! \n", __FUNCTION__, reg[i], defData[i]);

			//	Create New Setting
			if(!defData[i])
				setData[i] = defData[i] + 1;
			else
				setData[i] = defData[i] - 1;

			//	Config to New Setting
			if(fnPMICWrite(PF0100_I2C_BUS, PF0100_I2C_ADDRESS, reg[i], &setData[i], 1) != HK_SUCCESS)
				return HK_FAIL;

			//	Read Back
			if(fnPMICRead(PF0100_I2C_BUS, PF0100_I2C_ADDRESS, reg[i], &getData[i], 1) != HK_SUCCESS)
				return HK_FAIL;

			//	Compare
			if(getData[i] != setData[i])
				return HK_FAIL;

			//	Recover
			if(fnPMICWrite(PF0100_I2C_BUS, PF0100_I2C_ADDRESS, reg[i], &defData[i], 1) != HK_SUCCESS)
				return HK_FAIL;
		}
	}

	//	Boost Check
	{
		u8 defData[8];

		if(fnPMICRead(PF0100_I2C_BUS, PF0100_I2C_ADDRESS, REG_PF0100_SWBSTCTL, &defData[0], 1) != HK_SUCCESS)
				return HK_FAIL;

		//	Write & Read & Compare
		{
			u8 setData[8];
			u8 getData[8];

			//	Create New Setting
			if(!REG_BST_VOL(defData[0]))
				setData[0] = defData[0] + 1;
			else
				setData[0] = defData[0] - 1;

			//printf("\n  ### hk_dbg, %s : Default:0x%02x, New:0x%02x!!\n", __FUNCTION__, defData[0], setData[0]);

			if(fnPMICWrite(PF0100_I2C_BUS, PF0100_I2C_ADDRESS, REG_PF0100_SWBSTCTL, &setData[0], 1) != HK_SUCCESS)
					return HK_FAIL;

			if(fnPMICRead(PF0100_I2C_BUS, PF0100_I2C_ADDRESS, REG_PF0100_SWBSTCTL, &getData[0], 1) != HK_SUCCESS)
					return HK_FAIL;

			if(getData[0] != setData[0])
					return HK_FAIL;
		}

		//	Recover
		if(fnPMICWrite(PF0100_I2C_BUS, PF0100_I2C_ADDRESS, REG_PF0100_SWBSTCTL, &defData[0], 1) != HK_SUCCESS)
				return HK_FAIL;
	}

	//	LDO Check
	{
		u8 i;

		u8 reg[8]={REG_PF0100_VGEN1CTL, REG_PF0100_VGEN2CTL, REG_PF0100_VGEN3CTL, REG_PF0100_VGEN4CTL, REG_PF0100_VGEN5CTL, REG_PF0100_VGEN6CTL};
		u8 defData[8];
		u8 setData[8];
		u8 getData[8];

		for(i=0;i<6;i++)
		{
			//	Read Default Setting
			if(fnPMICRead(PF0100_I2C_BUS, PF0100_I2C_ADDRESS, reg[i], &defData[i], 1) != HK_SUCCESS)
				return HK_FAIL;

			//printf("### hk_dbg, %s : reg:0x%02x, data:0x%2x!! \n", __FUNCTION__, reg[i], defData[i]);

			//	Create New Setting
			if(!REG_VGENx_VOLT(defData[i]))
				setData[i] = defData[i] + 1;
			else
				setData[i] = defData[i] - 1;

			//printf("\n  ### hk_dbg, %s : setData[%d]:0x%02x!!\n", __FUNCTION__, i, setData[i] );

			//	Config to New Setting
			if(fnPMICWrite(PF0100_I2C_BUS, PF0100_I2C_ADDRESS, reg[i], &setData[i], 1) != HK_SUCCESS)
				return HK_FAIL;

			//	Read Back
			if(fnPMICRead(PF0100_I2C_BUS, PF0100_I2C_ADDRESS, reg[i], &getData[i], 1) != HK_SUCCESS)
				return HK_FAIL;

			//	Compare
			if(getData[i] != setData[i])
				return HK_FAIL;

			//	Recover
			if(fnPMICWrite(PF0100_I2C_BUS, PF0100_I2C_ADDRESS, reg[i], &defData[i], 1) != HK_SUCCESS)
				return HK_FAIL;
		}
	}

	//	SNVS Check
	{
		u8 defData[8];

		if(fnPMICRead(PF0100_I2C_BUS, PF0100_I2C_ADDRESS, REG_PF0600_VSNVSCTL, &defData[0], 1) != HK_SUCCESS)
				return HK_FAIL;

		//	Write & Read & Compare
		{
			u8 setData[8];
			u8 getData[8];

			//	Create New Setting
			if(!REG_VSNVS_VOL(defData[0]))
				setData[0] = defData[0] + 1;
			else
				setData[0] = defData[0] - 1;

			//printf("\n  ### hk_dbg, %s : Default:0x%02x, New:0x%02x!!\n", __FUNCTION__, defData[0], setData[0]);

			if(fnPMICWrite(PF0100_I2C_BUS, PF0100_I2C_ADDRESS, REG_PF0600_VSNVSCTL, &setData[0], 1) != HK_SUCCESS)
					return HK_FAIL;

			if(fnPMICRead(PF0100_I2C_BUS, PF0100_I2C_ADDRESS, REG_PF0600_VSNVSCTL, &getData[0], 1) != HK_SUCCESS)
					return HK_FAIL;

			if(getData[0] != setData[0])
					return HK_FAIL;
		}

		//	Recover
		if(fnPMICWrite(PF0100_I2C_BUS, PF0100_I2C_ADDRESS, REG_PF0600_VSNVSCTL, &defData[0], 1) != HK_SUCCESS)
				return HK_FAIL;
	}

	return HK_SUCCESS;
}
//------------------------------------------------

/*
 * date: 2016/10/05
 * modify : hk
 * description: Read pf0100
 */
u8 fnPF0100RegRead(u8 reg, u8 *data)
{
	u8 	buf[32];

	//	Read PMIC
	if(fnPMICRead(PF0100_I2C_BUS, PF0100_I2C_ADDRESS, reg, buf, 1) == HK_SUCCESS)
	{
		*data = buf[0];

		return HK_SUCCESS;
	}
	else
	{
		*data = 0xFF;

		return HK_FAIL;
	}

	return HK_SUCCESS;
}
//------------------------------------------------

/*
 * date: 2016/10/05
 * modify : hk
 * description: Write pf0100
 */
u8 fnPF0100RegWrite(u8 reg, u8 data)
{
	u8 	buf[32];

	buf[0] = data;

	//	Write PMIC
	if(fnPMICWrite(PF0100_I2C_BUS, PF0100_I2C_ADDRESS, reg, buf, 1) == HK_SUCCESS)
		return HK_SUCCESS;
	else
		return HK_FAIL;
}
//------------------------------------------------
