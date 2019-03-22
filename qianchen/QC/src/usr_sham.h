#ifndef __USR_SHARM_H__
#define __USR_SHARM_H__

#include "common.h"

#define DEVNO_BUF_SIZE		33
#define FW_VER_BUF_SIZE		64
#define GCC_VER_BUF_SIZE	64
#define LAN_NAME_BUF_SIZE	20
#define WAN_NAME_BUF_SIZE	20
#define WAN_MAC_BUF_SIZE	20
#define GW_ADDR_BUF_SIZE	32


#define DEVNO_LEN			32

typedef struct
{
	char devNo[DEVNO_BUF_SIZE];
	char fwv[FW_VER_BUF_SIZE];
	char gccv[GCC_VER_BUF_SIZE];
	char lan_name[LAN_NAME_BUF_SIZE];
	char gw[GW_ADDR_BUF_SIZE];
	char wan_name[WAN_NAME_BUF_SIZE];
	char wan_mac[WAN_MAC_BUF_SIZE];
	pthread_mutex_t lock;
} dev_share_t;

extern dev_share_t *dev_share;

#define GET_DEV_SHM_LOCK	pthread_mutex_lock(&dev_share->lock)
#define GET_DEV_SHM_UNLOCK	pthread_mutex_unlock(&dev_share->lock)
#define GET_DEV_NO			dev_share->devNo
#define GET_FW_VER			dev_share->fwv
#define GET_GCC_VER			dev_share->gccv
#define GET_LAN_NAME		dev_share->lan_name
#define GET_GW_ADDR			dev_share->gw
#define GET_WAN_NAME		dev_share->wan_name
#define GET_WAN_MAC			dev_share->wan_mac


int dev_share_init();

#endif

