#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <strings.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/ioctl.h>
#include "common.h"
#include "sFlashComm.h"

/*
 * date: 2016/07/25
 * modify : hk
 * description: Serial Flash Read Function
 */
u8 fnSFlashRead(char* dev, u8 *cmd, u8 *buf)
{
	return HK_SUCCESS;
}
//-----------------------------------------------------

/*
 * date: 2016/07/25
 * modify : hk
 * description: Serial Flash Write Function
 */
u8 fnSFlashWrite(char* dev, u8 addr, u8 reg, u8 *buf, u8 len)
{
	return HK_SUCCESS;
}
//-----------------------------------------------------

