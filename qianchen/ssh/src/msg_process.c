#include "includes.h"

int OnMessage(unsigned char* pbyData, int iLen)
{
	if (iLen < FRAME_MIN_LEN)
		return -1;
	
	if (pbyData[FRAME_HEADER_OFFSET] != FRAME_HEADER)
		return -1;
	
	switch (pbyData[FRAME_DST_OFFSET])
	{
		case FRAME_MODULE_INIT:
		{
			LOG_NORMAL_INFO("send msg to init.\n");
			SendMsgToInit((char *)pbyData, iLen);
			break;
		}
		case FRAME_MODULE_MONITOR:
		{
			LOG_NORMAL_INFO("send msg to monitor.\n");
			SendMsgToMonitor((char *)pbyData, iLen);
			break;
		}
		case FRAME_MODULE_FW_SET:
		{
			LOG_NORMAL_INFO("send msg to fw_set.\n");
			SendMsgToFwSet((char *)pbyData, iLen);
			break;
		}		
		case FRAME_MODULE_QCDOG:
		{
			LOG_NORMAL_INFO("send msg to qcdog.\n");
			SendMsgToQcdog((char *)pbyData, iLen);
			break;
		}
		case FRAME_MODULE_QCHTTPD:
		{
			LOG_NORMAL_INFO("send msg to qchttpd.\n");
			SendMsgToQchttpd((char *)pbyData, iLen);
			break;
		}
		case FRAME_MODULE_SSH:
		{
			break;
		}		
		case FRAME_MODULE_MITM:
		{
			LOG_NORMAL_INFO("send msg to mproxy_arm.\n");
			SendMsgToMitm((char *)pbyData, iLen);
			break;
		}
		case FRAME_MODULE_APCTRL:
		{
			LOG_NORMAL_INFO("send msg to apctrl.\n");
			SendMsgToApctrl((char *)pbyData, iLen);
			break;
		}
		default :
		{
			break;
		}
	}
	
	return 0;
}


