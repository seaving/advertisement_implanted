
#include "includes.h"

#define MSG_BLOCK	0
#define MSG_NOBLOCK	IPC_NOWAIT

// 消息
typedef struct {
	long mtype;
	char mtext[MAX_MESSAGE_LEN + 1];
} msg_info;

/**************************************************************************************************
* 函数功能：创建消息队列
* 入口参数：关键字
* 返回数值：消息标识符
* 调用函数：
**************************************************************************************************/
int CreatMsgQueue(key_t key)
{   
    int msqid;

    msqid = msgget(key, (IPC_CREAT | 0660));
    if (msqid == -1)
    {
        printf("cannot create message queue resource\r\n");
        return -1;
    }

	return msqid;
}

/**************************************************************************************************
* 函数功能：发送消息
* 入口参数：消息标识符、消息内容
* 返回数值：
* 调用函数：
**************************************************************************************************/
int SendMsg(int msqid,char* msg, int iLen)
{
	int iResult;
	msg_info buf;
	
	buf.mtype = 1;
	memcpy(buf.mtext, msg, iLen);

	iResult = msgsnd(msqid, &buf, iLen, MSG_NOBLOCK);
	if (iResult == -1)
	{
		 printf("cannot send message to the message queue\r\n");
	}

	return iResult;
}

/**************************************************************************************************
* 函数功能：接收消息
* 入口参数：消息标识符、消息内容
* 返回数值：
* 调用函数：
* 0x24 src dst len data 0x0d 0x0a
**************************************************************************************************/
int RecvMsg(int iQid, char* pcMsg, int len)
{
	int iResult = 0;
	int iMsgType = 0;

	if (len > MAX_MESSAGE_LEN)
		return 0;

	msg_info buffer;
	
	iResult = msgrcv(iQid, &buffer, len, iMsgType, MSG_BLOCK);
	if (iResult == -1)
	{
		return -1;
	}
	else
	{
		memcpy(pcMsg, buffer.mtext, iResult);
		pcMsg[iResult] = 0;
	}

	return iResult;
}

//--------------------------------------------------------------------------------------
// 消息队列相关
#define KEY_MSG_SSH     	0x10
#define KEY_MSG_INIT  		0x20
#define KEY_MSG_QCHTTPD     0x30
#define KEY_MSG_MONITOR     0x40
#define KEY_MSG_QCDOG     	0x50
#define KEY_MSG_MITM     	0x60
#define KEY_MSG_FW_SET     	0x70
#define KEY_MSG_APCTRL     	0x80

static int _ApctrlMsgQid = -1;
static int _SshMsgQid = -1;
//--------------------------------------------------------------------------------------

void InitMsgQueue(void)
{
	_SshMsgQid = CreatMsgQueue(KEY_MSG_SSH);
	_ApctrlMsgQid = CreatMsgQueue(KEY_MSG_APCTRL);
}

int SendMsgToSsh(char * pcMsg, int iLen)
{
    return SendMsg(_SshMsgQid, pcMsg, iLen);
}

int RecvApctrlMsg(char* pcMsg)
{
	return RecvMsg(_ApctrlMsgQid, pcMsg, MAX_MESSAGE_LEN);
}

