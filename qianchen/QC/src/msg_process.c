#include "includes.h"

int OnMessage(unsigned char* pbyData, int iLen)
{
	if (iLen < FRAME_MIN_LEN)
		return -1;
	
	if (pbyData[FRAME_HEADER_OFFSET] != FRAME_HEADER)
		return -1;

	LOG_NORMAL_INFO("SRC: %d\n", pbyData[FRAME_SRC_OFFSET]);

	if (pbyData[FRAME_DST_OFFSET] != FRAME_MODULE_MITM)
		return -1;

	int data_len = pbyData[FRAME_DATA_LEN_OFFSET];
	char data[data_len + 1];
	memcpy(data, pbyData, data_len);
	data[data_len] = '\0';
	
	switch (pbyData[FRAME_CMD_OFFSET])
	{

	}
	
	return 0;
}

int msg_clear_fwset()
{
	char msg[MAX_MESSAGE_LEN];
	return RecvFwsetMsg(msg);
}

int msg_send_fwset(char *data, int data_len, unsigned char src, unsigned char dst, unsigned char cmd)
{
	char buf[FRAME_BUF_SIZE(data_len)];
	int len = frame_package(buf, FRAME_BUF_SIZE(data_len), src, dst, cmd, data, data_len);
	if (len > 0)
		len = SendMsgToFwset(buf, len);
	return len;
}

int msg_send(char *data, int data_len, unsigned char src, unsigned char dst, unsigned char cmd)
{
	char buf[FRAME_BUF_SIZE(data_len)];
	int len = frame_package(buf, FRAME_BUF_SIZE(data_len), src, dst, cmd, data, data_len);
	if (len > 0)
		len = SendMsgToSsh(buf, len);
	return len;
}

