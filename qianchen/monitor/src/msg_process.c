#include "includes.h"


int OnMessage(unsigned char* pbyData, int iLen)
{
	if (iLen < FRAME_MIN_LEN)
		return -1;
	
	if (pbyData[FRAME_HEADER_OFFSET] != FRAME_HEADER)
		return -1;

	if (pbyData[FRAME_DST_OFFSET] != FRAME_MODULE_MONITOR)
		return -1;

	char cmd[64] = {0};
	switch (pbyData[FRAME_CMD_OFFSET])
	{
		case FRAME_CMD_RESTART_PROGRAM:
		{
			EXECUTE_CMD(cmd, "killall mproxy_arm");
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

