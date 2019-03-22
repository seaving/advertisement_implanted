#ifndef __EVHTTPD_FILTER_CONFIG_H_
#define __EVHTTPD_FILTER_CONFIG_H_

#include "config.h"
#include "evhttpd_filter.h"

filter_list session_wlist[] = {
	{ WEB_LOGIN_PATH },
	{ "/css" },
	{ "/img" },
	{ "/js" },
	{ "/script" },
	{ "/layui" },
	{ WEB_LOGIN_CHECK_PATH },
	{ NULL }
};

filter_list session_blist[] = {
	{ "/" },
	{ "/html" },
	{ NULL }
};

filter_list passwd_list[] = {
	{ WEB_LOGIN_CHECK_PATH },
	{ NULL }
};

#endif

