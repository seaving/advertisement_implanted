#ifndef __CONFIG_H__
#define __CONFIG_H__

#define VIR_NET_DEV		"ifb0"
#define LAN_DEV			"br-lan"
#define LAN_GUEST_DEV	"br-guest"

#define CHANNEL			"100001"
#define CHANNEL_LEN		6

#define WAN_DEV		VIR_NET_DEV

#define SERVER_HOST		"www.qcwifi.ltd"


#define WEB_SERVER_PORT	80

#define	WEB_DOC_BASE	"/tmp/www"

#define WEB_CHAR_SET	"utf-8"


#define WEB_HOME_PATH		"/html/home.html"
#define WEB_LOGIN_PATH		"/html/login.html"
#define WEB_SET_PASSWD		"/html/setpwd.html"
#define WEB_LOGIN_EXIT		"/html/login_exit.html"

#define WEB_RESET_PASSWD	"/resetpwd"
#define WEB_LOGIN_CHECK_PATH	"/dologin"

#define WEB_SUCCESS_PATH	"/html/success.html"
#define WEB_ERROR_PATH		"/html/error.html"


#define AC_SERVER_PORT	61000
#define AP_CLIENT_PORT	61001

// rm qchttpd;killall qchttpd;wget http://134.227.227.110/rp/qianchen/qc_httpd/hex/qchttpd;wget http://134.227.227.110/rp/qianchen/qc_httpd/www.gz;mv www.gz /etc/app/;chmod +x qchttpd;./qchttpd
#endif


