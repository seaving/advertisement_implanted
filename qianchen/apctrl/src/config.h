#ifndef __CONFIG_H__
#define __CONFIG_H__

#define DEV_NO_FILE				"/etc/config/devNo"
#define FIRMWARE_VERSION_FILE	"/etc/app/firmware_version"
#define GCC_VERSION_FILE 		"/etc/app/gcc_version"

//-------------------------------------
#define PROCESS_NUM			1							//进程数量
#define MAX_EVENTS			2048

#define MAX_LISTEN			300
#define MAX_CLIENT			500

//单进程处理最多客户端数量
//#define MAX_CONNECTIONS		2048

#define THREAD_NUM			500							//单进程中线程数量

//--------------------------------------

//#define WAN_DEV		"eth0.1" //已经改为动态获取
#define LAN_DEV		"br-lan"

#define MITM_PORT				60000					//代理劫持服务器端口配置
#define AUTHEN_CHECK_PORT		40000	 				//认证服务器端口配置
#define AUTHEN_JUMP_PORT		50000 					//认证跳转服务器端口配置


/**认证上网时间(sec)*/
#define _1_SEC_ 	1
#define _1_MIN_		60 * _1_SEC_
#define _1_HOUR_	60 * _1_MIN_

#define AUTHEN_INTERVAL			2 *  _1_MIN_ // 2 * _1_HOUR_
#define AUTHEN_JUMP_INTERVAL	10 * _1_SEC_

/**版本更新间隔*/ 
#define UPDATE_TIME	 1 * _1_HOUR_

/**认证服务器主机地址*/
#define AUTHEN_HOST				"121.196.215.235"
#define AUTHEN_HOST_LEN			15
#define AUTHEN_URL				"http://121.196.215.235/auth/index.html"
#define AUTHEN_URL_FOR_WECHAT	"http%3a%2f%2f121.196.215.235%3fseaving_auth_flag%3dcom.auth.success.www"//"http://121.196.215.235?seaving_auth_flag=com.auth.success.www"
/****************END***************/

#endif

