#ifndef __CONF_H__
#define __CONF_H__

#define MAX_CONN_ARRAY_SIZE (131072) 	//1024 * 128
#define	CONN_BUF_LEN		(2048)
#define	MAX_BUF_READ_LEN	(1791) 		//CONN_BUF_LEN - 257
#define	MAX_PKG_LEN			(1500)


#define SERVER_PORT			9000
#define SOCKET_TIMEOUT_S	8
#define WORKER_NUM			4
#define CONN_NUM			1024
#define USER_NUM			200
#define FILE_NUM			200


#endif



