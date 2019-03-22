#ifndef __MSG_H__
#define __MSG_H__

#define MAX_MESSAGE_LEN   64

int SendMsgToSsh(char* pcMsg, int iLen);
int RecvApctrlMsg(char* pcMsg);
void InitMsgQueue(void);


#endif

