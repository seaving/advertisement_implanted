#ifndef __MSG_H__
#define __MSG_H__

#define MAX_MESSAGE_LEN   64

int SendMsgToSsh(char * pcMsg, int iLen);

int SendMsgToInit(char * pcMsg, int iLen);

int SendMsgToQchttpd(char * pcMsg, int iLen);

int SendMsgToMonitor(char * pcMsg, int iLen);

int SendMsgToQcdog(char * pcMsg, int iLen);

int SendMsgToMitm(char * pcMsg, int iLen);

int SendMsgToFwSet(char * pcMsg, int iLen);

int SendMsgToApctrl(char * pcMsg, int iLen);

int RecvSshMsg(char* pcMsg);

void InitMsgQueue(void);


#endif

