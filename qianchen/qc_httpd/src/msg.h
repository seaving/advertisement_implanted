#ifndef __MSG_H__
#define __MSG_H__

#define MAX_MESSAGE_LEN   64

int SendMsgToSsh(char* pcMsg, int iLen);
int RecvQchttpdMsg(char* pcMsg);
void InitMsgQueue(void);
int SendMsgToInit(char * pcMsg, int iLen);


#endif

