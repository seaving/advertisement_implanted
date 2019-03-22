#include "includes.h"

#define KEY_SHM_DEV_SHR    0x60

static int _dev_share_id = -1;

dev_share_t *dev_share = NULL;

void _get_device_info_call(char *buf, void *info)
{
	if (buf && info)
	{
		memcpy((char *)info, buf, strlen(buf));
	}
}

int get_device_info(char *cmd, char *info)
{
	if (popen_cmd(cmd, _get_device_info_call, info) < 0)
	{
		return -1;
	}
	
	LOG_WARN_INFO("device info: %s\n", info);

	return 0;
}

int dev_share_init()
{
	dev_share = (dev_share_t *)InitSham(&_dev_share_id, sizeof(dev_share_t), KEY_SHM_DEV_SHR);
	if (dev_share == (dev_share_t *)(-1))
	{
		LOG_ERROR_INFO("init sham error!\n");
		return -1;
	}

	pthread_mutex_init(&dev_share->lock, NULL);

	GET_DEV_SHM_LOCK;
	
	memset(GET_DEV_NO, 0, DEVNO_BUF_SIZE);
	safe_read_file(DEV_NO_FILE, GET_DEV_NO, DEVNO_BUF_SIZE - 1);
	del_tab_newline(GET_DEV_NO);
	printf("dev no: %s\n", GET_DEV_NO);
	
	memset(GET_FW_VER, 0, FW_VER_BUF_SIZE);
	safe_read_file(FIRMWARE_VERSION_FILE, GET_FW_VER, FW_VER_BUF_SIZE - 1);
	del_tab_newline(GET_FW_VER);
	printf("fw ver: %s\n", GET_FW_VER);
	
	memset(GET_GCC_VER, 0, GCC_VER_BUF_SIZE);
	safe_read_file(GCC_VERSION_FILE, GET_GCC_VER, GCC_VER_BUF_SIZE - 1);
	del_tab_newline(GET_GCC_VER);
	printf("gcc ver: %s\n", GET_GCC_VER);

	sprintf(GET_LAN_NAME, "%s", "br-lan");
	get_device_info("uci get network.wan.ifname", GET_WAN_NAME);
	get_device_info("uci get network.lan.ipaddr", GET_GW_ADDR);

	printf("wan.ifname: %s\n", GET_WAN_NAME);
	printf("lan.ifname: %s\n", GET_LAN_NAME);
	printf("gw addr: %s\n", GET_GW_ADDR);

	get_local_mac(GET_WAN_MAC, GET_WAN_NAME);
	printf("wan man: %s\n", GET_WAN_MAC);

	GET_DEV_SHM_UNLOCK;

	return 0;
}



