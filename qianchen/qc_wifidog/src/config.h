#ifndef __CONFIG_H__
#define __CONFIG_H__

#define MPROXY_ARM_PORT								60000
#define QC_DOG_HTTP_REDIRECT_SERVER_PORT			60001
#define QC_DOG_HTTPS_REDIRECT_SERVER_PORT			60002
#define QC_DOG_AUTH_SERVER_PORT						60003
#define QC_DOG_WEB_SERVER_PORT						60004

#define	DEFAULT_CA_CRT_FILE		"/tmp/app/apfree.ca"
#define	DEFAULT_SVR_CRT_FILE	"/tmp/app/apfree.crt"
#define	DEFAULT_SVR_KEY_FILE	"/tmp/app/apfree.key"

#define DEFAULT_HTMLMSGFILE 			"/tmp/app/wifidog/wifidog-msg.html"
#define DEFAULT_REDIRECTFILE 			"/tmp/app/wifidog/wifidog-redir.html"
#define	DEFAULT_INTERNET_OFFLINE_FILE	"/tmp/app/wifidog/internet-offline.html"
#define	DEFAULT_AUTHSERVER_OFFLINE_FILE	"/tmp/app/wifidog/authserver-offline.html"

#define LOADING_GIF_PATH	"/tmp/app/loading.gif"

#if 0
#define CA_FILE				"/tmp/app/CAcert.pem"
#define PRIVATE_FILE		"/tmp/app/privkey.pem"
#endif

#define LOADING_URI			"/loading.gif"

#define DEVNO_FILE			"/etc/config/devno.conf"

#define AUTH_SERVER_HOST	"www.qcwifi.ltd"
#define AUTH_SERVER_PORT	80

#define AUTH_LOGIN	"/rapp/authentication/wechat/login" //"/auther/login"
#define AUTH_LOGOUT	"" //"/auther/logout"
#define AUTH_AUTH	"/rapp/authentication/wechat/confim/%s/%s" //第一个%s是id，第二个是token

#define TEMP_ACCEPT_TIME		30
#define AUTH_VAILD_TIME			3600 * 12
#define DEFAULT_CHECKINTERVAL 	5

//rm qcdog ;killall qcdog ;wget http://134.227.0.110/rp/qianchen/qc_wifidog/hex/qcdog; chmod +x qcdog;./qcdog

#endif

