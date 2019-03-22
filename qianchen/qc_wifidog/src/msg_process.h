
#ifndef __MSG_PROCESS_H__
#define __MSG_PROCESS_H__

int OnMessage(unsigned char* pbyData, int iLen);
int msg_send(char *data, int data_len, unsigned char src, unsigned char dst, unsigned char cmd);

#endif


