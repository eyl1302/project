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
#include "eeti.h"

//	Internal Function

//	Function Pointer
u8 fnEETIInit(void);
u8 fnEETITest(void);
u8 fnEETIRegRead(u8 reg, u8 *data);
u8 fnEETIRegWrite(u8 reg, u8 data);

TPANEL_FUNC_T eeti =
{
	fnEETIInit,
	fnEETITest,
	fnEETIRegRead,
	fnEETIRegWrite,
};

/*
 * date: 2016/10/14
 * modify : hk
 * description: Get Raw Coordinate
 */
u8 fnEETIGetRawContact(TOUCH_CONTACT_T *tp, u8 len)
{
	u8					idx = 0;
	s32 				fd;
    char 				name[256] = "Unknown";
    struct input_event 	ev;

	//	Open Device
    fd = open(EVENT_DEVICE, O_RDONLY);

    if (fd == -1)
    {
    	printf("\n ### hk_dbg, %s : %s is not a vaild device!!\n", __FUNCTION__, EVENT_DEVICE);
        	return HK_FAIL;
    }

    ioctl(fd, EVIOCGNAME(sizeof(name)), name);

    //printf("\n  ### hk_dbg, %s : Reading from:\n", __FUNCTION__);
    //printf("\n  ### hk_dbg, %s : device file = %s\n", __FUNCTION__, EVENT_DEVICE);
    //printf("\n  ### hk_dbg, %s : device name = %s\n", __FUNCTION__, name);

	for (;;)
	{
        const size_t ev_size = sizeof(struct input_event);
        ssize_t size;

        /* TODO: use select() */
        size = read(fd, &ev, ev_size);

        //printf("\n  ### hk_dbg, %s : size:%d!!\n", __FUNCTION__, size);

        if (size < ev_size)
        {
        	printf("### hk_dbg, %s : Error size when reading!!\n", __FUNCTION__);

            close(fd);

    		return HK_FAIL;
        }

		/* TODO: convert value to pixels */
        if (ev.type == EVENT_TYPE && (ev.code == EVENT_CODE_X || ev.code == EVENT_CODE_Y))
        {
            //printf("\n  ### hk_dbg, %s : %s = %d\n", __FUNCTION__, ev.code == EVENT_CODE_X ? "X" : "Y", ev.value);

            if(ev.code == EVENT_CODE_X)
            {
            	tp->x[idx] = ev.value;

            	continue;
            }
			else
				tp->y[idx] = ev.value;

			idx++;

            if(idx >= len)
            	break;
        }
    }

    close(fd);

	return HK_SUCCESS;
}
//------------------------------------------------

/*
 * date: 2016/10/14
 * modify : hk
 * description: Initial EETI
 */
u8 fnEETIInit(void)
{
	return HK_SUCCESS;
}
//------------------------------------------------

/*
 * date: 2016/10/14
 * modify : hk
 * description: Auto Test EETI
 */
u8 fnEETITest(void)
{
	u8				idx;
	u8 				len;
	s32				x,y;
	TOUCH_CONTACT_T tp;

	len = 5;

	//	Divide Touch Panel into 4 Block
	//	  	|---------|---------|
	// (0,0)|---------|---------|
	//	  	|         |         |
	//	  	|  U & L  |  U & R  |
	//	  	|   (1)   |   (3)   |
	//	  	|         |         |
	//	  	|---------|---------|
	//	  	|         |         |
	//	  	|  D & L  |  D & R  |
	//	  	|   (2)   |   (4)   |
	//	  	|         |         |
	//	  	|---------|---------|(479, 639)
	//    	|	 Simpad Plus	|
	//		|---------|---------|

	//	4 Blocks
	printf("\n  Touch Panel & 4 Blocks " );
	printf("\n  ||=========|=========|| ");
	printf("\n  ||         o         || ");
	printf("\n  ||=========|=========|| ");
	printf("\n  ||         |         || ");
	printf("\n  ||  U & L  |  U & R  || ");
	printf("\n  ||   (1)   |   (3)   || ");
	printf("\n  ||         |         || ");
	printf("\n  ||=========|=========|| ");
	printf("\n  ||         |         || ");
	printf("\n  ||  D & L  |  D & R  || ");
	printf("\n  ||   (2)   |   (4)   || ");
	printf("\n  ||         |         || ");
	printf("\n  ||=========|=========|| ");
	printf("\n  ||      Simpad Plus  || ");
	printf("\n  ||=========|=========||\n ");

	for(idx=0;idx<4;idx++)
	{
		printf("\n  ## hk_dbg, %s : Please touch the %s block of screen...\n", __FUNCTION__, PANEL_BOLCK(idx));

		//	Get Raw Coordinate
		fnEETIGetRawContact(&tp, len);

		//	Average Calculation
		{
			u8 i;

			x = 0;
			y = 0;

			for(i=0;i<len;i++)
			{
				//printf("\n  ### hk_dbg, %s : x:%d, y:%d\n", __FUNCTION__, tp.x[i], tp.y[i]);

				x+=tp.x[i];
				y+=tp.y[i];
			}

			//	Average
			x = x / len;
			y = y / len;

			//printf("\n  ### hk_dbg, %s : Average of x:%d, y:%d\n", __FUNCTION__, x, y);
		}

		//	Convert
		x = CONTACT_CONVERT_X(x);
		y = CONTACT_CONVERT_Y(y);

		printf("\n  ### hk_dbg, %s : x:%d, y:%d!!\n", __FUNCTION__, x, y);

		{
			u16 upX, upY;
			u16 lowX, lowY;

			switch(idx)
			{
				case TB_UL:
					lowX = RSLN_X_MIN;
					lowY = RSLN_Y_MIN;
					upX = RSLN_X_HALF;
					upY = RSLN_Y_HALF;
				break;

				case TB_DL:
					lowX = RSLN_X_MIN;
					lowY = RSLN_Y_HALF;
					upX = RSLN_X_HALF;
					upY = RSLN_Y_MAX;
				break;

				case TB_UR:
					lowX = RSLN_X_HALF;
					lowY = RSLN_Y_MIN;
					upX = RSLN_X_MAX;
					upY = RSLN_Y_HALF;
				break;

				case TB_DR:
					lowX = RSLN_X_HALF;
					lowY = RSLN_Y_HALF;
					upX = RSLN_X_MAX;
					upY = RSLN_Y_MAX;
				break;

				default:
					printf("\n  ### hk_dbg, %s:  Out of blcok range :%d!!\n", __FUNCTION__, idx);
					return HK_FAIL;
			}

			//	Coordinate X Check
			if((x < lowX) || (x > upX))
		    	return HK_FAIL;

			//	Coordinate Y Check
		    if((y < lowY) || (y > upY))
            	return HK_FAIL;
		}
	}

    return HK_SUCCESS;
}
//------------------------------------------------

/*
 * date: 2016/10/14
 * modify : hk
 * description: Read EETI
 */
u8 fnEETIRegRead(u8 reg, u8 *data)
{
	u8 	buf[32];

	//	Read eeti
	if(fnTPanelRead(EETI_I2C_BUS, EETI_I2C_ADDRESS, reg, buf, 1) == HK_SUCCESS)
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
 * date: 2016/10/14
 * modify : hk
 * description: Write EETI
 */
u8 fnEETIRegWrite(u8 reg, u8 data)
{
	u8 	buf[32];

	buf[0] = data;

	//	Write eeti
	if(fnTPanelWrite(EETI_I2C_BUS, EETI_I2C_ADDRESS, reg, buf, 1) == HK_SUCCESS)
		return HK_SUCCESS;
	else
		return HK_FAIL;
}
//------------------------------------------------
