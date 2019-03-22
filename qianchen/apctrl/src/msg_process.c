#include "includes.h"

int OnMessage(unsigned char* pbyData, int iLen)
{
	if (iLen < FRAME_MIN_LEN)
		return -1;
	
	if (pbyData[FRAME_HEADER_OFFSET] != FRAME_HEADER)
		return -1;

	LOG_NORMAL_INFO("SRC: %d\n", pbyData[FRAME_SRC_OFFSET]);

	if (pbyData[FRAME_DST_OFFSET] != FRAME_MODULE_APCTRL)
		return -1;

	int data_len = pbyData[FRAME_DATA_LEN_OFFSET];
	char data[data_len + 1];
	memcpy(data, &pbyData[FRAME_DATA_OFFSET], data_len);
	data[data_len] = '\0';
	
	switch (pbyData[FRAME_CMD_OFFSET])
	{
		case FRAME_CMD_SET_DEV_NO:
		{
			GET_DEV_SHM_LOCK;
			printf("onmessege: new dev no: %s\n", data);
			snprintf(GET_DEV_NO, DEVNO_BUF_SIZE, "%s", data);
			GET_DEV_SHM_UNLOCK;
			break;
		}
		case FRAME_CMD_FW_VER:
		{
			GET_DEV_SHM_LOCK;
			printf("onmessege: new fw ver: %s\n", data);
			snprintf(GET_FW_VER, FW_VER_BUF_SIZE, "%s", data);
			GET_DEV_SHM_UNLOCK;		
			break;
		}
		case FRAME_CMD_GCC_VER:
		{
			GET_DEV_SHM_LOCK;
			printf("onmessege: new gcc ver: %s\n", data);
			snprintf(GET_GCC_VER, GCC_VER_BUF_SIZE, "%s", data);
			GET_DEV_SHM_UNLOCK;
			break;
		}		
		case FRAME_CMD_LAN_NAME:
		{
			GET_DEV_SHM_LOCK;
			printf("onmessege: lan name: %s\n", data);
			snprintf(GET_LAN_NAME, LAN_NAME_BUF_SIZE, "%s", data);
			GET_DEV_SHM_UNLOCK;
			break;
		}
		case FRAME_CMD_WAN_NAME:
		{
			GET_DEV_SHM_LOCK;
			printf("onmessege: wan name: %s\n", data);
			snprintf(GET_WAN_NAME, WAN_NAME_BUF_SIZE, "%s", data);
			GET_DEV_SHM_UNLOCK;
			break;
		}
		case FRAME_CMD_WAN_MAC:
		{
			GET_DEV_SHM_LOCK;
			printf("onmessege: wan mac: %s\n", data);
			snprintf(GET_WAN_MAC, WAN_MAC_BUF_SIZE, "%s", data);
			GET_DEV_SHM_UNLOCK;
			break;
		}		
		case FRAME_CMD_GW_ADDR:
		{
			GET_DEV_SHM_LOCK;
			printf("onmessege: gw addr: %s\n", data);
			snprintf(GET_GW_ADDR, GW_ADDR_BUF_SIZE, "%s", data);
			GET_DEV_SHM_UNLOCK;
			break;
		}
		default :
		{
			break;
		}
	}
	
	return 0;
}

int msg_send(char *data, int data_len, unsigned char src, unsigned char dst, unsigned char cmd)
{
	char buf[FRAME_BUF_SIZE(data_len)];
	int len = frame_package(buf, FRAME_BUF_SIZE(data_len), src, dst, cmd, data, data_len);
	if (len > 0)
		SendMsgToSsh(buf, len);
	return len;
}

