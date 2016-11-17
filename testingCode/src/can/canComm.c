#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <strings.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/ioctl.h>
#include <fcntl.h>
#include "i2c-dev.h"
#include "common.h"
#include "system.h"
#include "can.h"
#include "canComm.h"

/*
 * date: 2016/10/20
 * modify : hk
 * description: CAN Read Function
 */
u8 fnCANRead(u8 *buf, u8 len)
{
	return HK_SUCCESS;
}
//------------------------------------------------

/*
 * date: 2016/10/20
 * modify : hk
 * description: CAN Write Function
 */
u8 fnCANWrite(u8 *buf, u8 len)
{
	return HK_SUCCESS;
}
//------------------------------------------------
