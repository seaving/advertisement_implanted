#ifndef __MSG_H__
#define __MSG_H__

#define MAX_MESSAGE_LEN   64

int SendMsgToSsh(char* pcMsg, int iLen);
int RecvMitmMsg(char* pcMsg);
void InitMsgQueue(void);
int RecvFwsetMsg(char* pcMsg);

int SendMsgToFwset(char * pcMsg, int iLen);

#endif

