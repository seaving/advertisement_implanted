#include "includes.h"
#if 0

/**
	rule: mangle
		-------all ip not port 80 to newchain
	rule: newchain
		------- 134.227.227.2 drop (when after auth to accept)
		------- 134.227.227.3 drop (when after auth to accept)
		------- 134.227.227.4 drop (when after auth to accept)
		------- ...
**/


#define MANGLE_NEW_CHAIN	"seaving"


#define AUTHEN_CONF_DIR		"/tmp/authen/"

#define IPTABLES_CMD_BUF_SIZE	255

//呼起微信公众号后点击链接wifi发生的动作
///?seaving_auth_flag=com.auth.success.www&openId=ooi4GwBby2uxFtrNNy_zZBCcJbQ4&tid=010001b0bda59a82040750c2ac7085454d97e93fe1c9d464224652&extend=null&timestamp=1477375649&sign=19e61920f2d35d7675e7df3ea7838193
#define AUTH_SECCESS_FLAG	"?seaving_auth_flag=com.auth.success.www"
#define AUTH_SECCESS_FLAG_LEN	39

#define AUTH_JUMP_WEB_PAGE_FLAG	"/seaving_auth?seaving_jumpweb=success&seaving=hi"
#define AUTH_JUMP_WEB_PAGE_FLAG_LEN	48

#define AUTH_GET_PHONE_MAC_FLAG	"/seaving_auth?seaving_jumpweb=getPhoneMac&seaving=hi"
#define AUTH_GET_PHONE_MAC_FLAG_LEN	52

static pthread_mutex_t auth_lock = PTHREAD_MUTEX_INITIALIZER;

typedef struct Auth
{
	int authflag; // 0: 未认证, 1: 认证放行
	int start;
	int validTime;
	int jumpflag;
	char mac[20];
} auth_t;
auth_t auth_ip[255];

void create_firewall_chain();

void clean_auth_ip(int who)
{
#if 0
	auth_ip[who].authflag = 0;
	auth_ip[who].start = 0;
	auth_ip[who].validTime = 0;
	auth_ip[who].jumpflag = 0;
#endif
	memset(&auth_ip[who], 0, sizeof(auth_t));
}

int fill_authen(int who)
{
	int ret = -1;
	int size = sizeof(auth_t);
	memset(&auth_ip[who], 0, size);
	char file[32];
	sprintf(file, "%s%d", AUTHEN_CONF_DIR, who);

	if((access(file, F_OK)))
	{
		return -1;
	}

	FILE *fp = fopen(file, "rb+");
	if (fp == NULL)
	{
		LOG_PERROR_INFO("fopen \"%s\" error!", file);
		ret = -1;
	}
	else
	{
		if (fread(&auth_ip[who], size, 1, fp) <= 0)
		{
			LOG_PERROR_INFO("fread \"%s\" error!", file);
			ret = -1;
		}
		ret = 0;
		LOG_NORMAL_INFO("---------------------------------------\n");
		LOG_NORMAL_INFO("%s.%d.authflag  : %d\n", 	get_netdev_lan_segment(), who, auth_ip[who].authflag);
		LOG_NORMAL_INFO("%s.%d.validTime : %d\n", 	get_netdev_lan_segment(), who, auth_ip[who].validTime);
		LOG_NORMAL_INFO("%s.%d.start     : %d\n", 	get_netdev_lan_segment(), who, auth_ip[who].start);
		LOG_NORMAL_INFO("---------------------------------------\n");
	}
	if (fp)
	{
		fclose(fp);
		fp = NULL;
	}
	return ret;
}

#define RULE_OFFSET		1
int get_rule_num(int who)
{
	return who - RULE_OFFSET;
}

void mangle_jump_rule(char *strport)
{
	char iptables[IPTABLES_CMD_BUF_SIZE];
	sprintf(iptables, "iptables -t mangle -I PREROUTING -w -p tcp -m multiport ! --dports %s -j %s",
						strport, MANGLE_NEW_CHAIN);
	LOG_NORMAL_INFO("AUTH %s\n", iptables);
	system(iptables);
}

void mangle_drop_rule(int index, int who, char *op)
{
	char iptables[IPTABLES_CMD_BUF_SIZE];
	sprintf(iptables, "iptables -t mangle %s %s %d -i %s -w -p tcp -s %s.%d -j DROP",
				op, MANGLE_NEW_CHAIN, index, get_netdev_lan_name(), get_netdev_lan_segment(), who);
	LOG_NORMAL_INFO("AUTH %s\n", iptables);
	system(iptables);	
}

void mangle_accept_rule(int index, int who, char *op)
{
	char iptables[IPTABLES_CMD_BUF_SIZE];
	sprintf(iptables, "iptables -t mangle %s %s %d -i %s -w -p tcp -s %s.%d -j ACCEPT",
				op, MANGLE_NEW_CHAIN, index, get_netdev_lan_name(), get_netdev_lan_segment(), who);
	LOG_NORMAL_INFO("AUTH %s\n", iptables);
	system(iptables);
}

void mangle_accept_url_rule(char *url, char *dir)
{
	char iptables[IPTABLES_CMD_BUF_SIZE];
	sprintf(iptables, "iptables -t mangle -I %s -i %s -w -p tcp %s %s -j ACCEPT",
				"PREROUTING", get_netdev_lan_name(), dir, url);
	LOG_NORMAL_INFO("AUTH %s\n", iptables);
	system(iptables);	
}

void mangle_accept_otherIp_rule()
{
	char iptables[IPTABLES_CMD_BUF_SIZE];
	sprintf(iptables, "iptables -t mangle -I %s -i %s -w -p tcp ! -s %s.0/24 -j ACCEPT",
				"PREROUTING", get_netdev_lan_name(), get_netdev_lan_segment());
	LOG_NORMAL_INFO("AUTH %s\n", iptables);
	system(iptables);	
}

void mangle_accept_port_rule(int port)
{
	char iptables[IPTABLES_CMD_BUF_SIZE];
	sprintf(iptables, "iptables -t mangle -I %s -i %s -w -p tcp --dport %d -j ACCEPT",
				"PREROUTING", get_netdev_lan_name(), port);
	LOG_NORMAL_INFO("AUTH %s\n", iptables);
	system(iptables);	
}

void auth_struct_init()
{
	memset(auth_ip, 0, sizeof(auth_t) * 255);
	mkdir(AUTHEN_CONF_DIR, 0700);
	int who = 0;
	for (who = 2; who < 255; who ++)
	{
		fill_authen(who);
	}
}

void auth_firewall_init()
{
	create_firewall_chain();

	//22端口作为SSH调试用的，正式发布版本22端口需要禁止
	mangle_jump_rule("80,8080,22,9999");

	//放行微信认证URL
	mangle_accept_url_rule("wifi.weixin.qq.com", "-d");

	//放行所有的443端口
	//mangle_accept_port_rule(443);

	//放行非局域网的IP
	mangle_accept_otherIp_rule();
}

int get_sub_segment(char *phoneIp)
{
	int who = -1;
	int i = strlen(phoneIp);
	for ( ; i >= 0; i --)
	{
		if (phoneIp[i] == '.')
		{
			who = atoi(&phoneIp[i+1]);
			return who;
		}
	}
	return -1;
}

void delete_auth_conf(int who)
{
	char file[32];
	sprintf(file, "%s%d", AUTHEN_CONF_DIR, who);
	
	if((access(file, F_OK)) == 0)
	{
		unlink(file);
	}
}

void save_auth_conf(int who)
{
	char file[32];
	sprintf(file, "%s%d", AUTHEN_CONF_DIR, who);
	//unlink(file);
	FILE *fp = fopen(file, "wb+");
	if (fp == NULL)
	{
		LOG_PERROR_INFO("fopen \"%s\" error!", file);
	}
	else
	{
		int ret = fwrite(&auth_ip[who], sizeof(auth_t), 1, fp);
		if (ret <= 0 )
		{
			LOG_PERROR_INFO("fwrite \"%s\" error, ret = %d!", file, ret);
		}
	}	
	if (fp)
	{
		fclose(fp);
		fp = NULL;
	}
}

void create_firewall_chain()
{	
	pthread_mutex_lock(&auth_lock);

	char iptables[IPTABLES_CMD_BUF_SIZE];
	sprintf(iptables, "iptables -t mangle -N %s", MANGLE_NEW_CHAIN);
	LOG_NORMAL_INFO("AUTH %s\n", iptables);
	system(iptables);
#if 1
	int who = 0;
	int index = 0;
	for (who = 2; who < 255; who ++)
	{
		index = get_rule_num(who);
		if (! auth_ip[who].authflag)
			mangle_drop_rule(index, who, "-I");
	#if 1
		else
		{
			mangle_accept_rule(index, who, "-I");
		}
	#else
		else if (GetSystemTime_Sec() - auth_ip[who].start < auth_ip[who].validTime)
		{
			LOG_NORMAL_INFO("auth: %d\n", auth_ip[who].authflag);
			LOG_NORMAL_INFO("start: %d\n", auth_ip[who].start);
			LOG_NORMAL_INFO("validTime: %d\n", auth_ip[who].validTime);
			mangle_accept_rule(index, who, "-I");
		}
		else
		{
			clean_auth_ip(who);
			delete_auth_conf(who);
			mangle_drop_rule(index, who, "-I");
		}
	#endif
	}
#endif
	pthread_mutex_unlock(&auth_lock);
}

void *check_validTime(void *arg)
{
	pthread_detach(pthread_self());

	int who = 0;

	int sec = 0;

	while (1)
	{
		for (who = 2; who < 255; who ++)
		{
			sec = GetSystemTime_Sec();
			if (auth_ip[who].authflag)
			{
				if (sec - auth_ip[who].start >= auth_ip[who].validTime)
				{
					auth_ip[who].start = GetSystemTime_Sec();
					
					char ip[20] = {0};
					char mac[20] = {0};
					pthread_mutex_lock(&auth_lock);
					sprintf(ip, "%s.%d", get_netdev_lan_segment(), who);
					get_sta_host_mac(ip, mac, 19);
					//printf("arpmac:%s   authmac:%s\n", mac, auth_ip[who].mac);
					//if ((sec - auth_ip[who].start) >= auth_ip[who].validTime)
					if (strcmp(mac, auth_ip[who].mac)) //不是同一个手机或者手机下线
					{
						LOG_NORMAL_INFO("arpmac:%s   authmac:%s\n", mac, auth_ip[who].mac);
						LOG_NORMAL_INFO("%d : offline !\n", who);
						delete_auth_conf(who);
						clean_auth_ip(who);
						mangle_drop_rule(get_rule_num(who), who, "-R");
					}
					pthread_mutex_unlock(&auth_lock);
				}
			}
			else
			{
				pthread_mutex_lock(&auth_lock);
				if (!auth_ip[who].authflag && auth_ip[who].jumpflag)
				{
					if ((sec - auth_ip[who].start) >= auth_ip[who].validTime)
					{
						LOG_NORMAL_INFO("%d: not auth in %d sec!\n", who, auth_ip[who].validTime);
						clean_auth_ip(who);
					}
				}
				pthread_mutex_unlock(&auth_lock);
			}
		}
		sleep(3);
	}
	
	return NULL;
}

int check_validTime_thread()
{
	pthread_t thread;
	if (pthread_create(&thread, NULL, check_validTime, NULL) != 0)
	{
		LOG_PERROR_INFO("pthread_create error.");
		return -1;
	}
	return 0;
}

int do_auth(int phonefd, char *host, int host_len, char *uri, int uri_len, char *header, int header_len)
{
	int who = -1;
	char phoneIp[32] = {0};
	get_peer_name(phonefd, phoneIp, NULL);
	who = get_sub_segment(phoneIp);
	if (who < 0)
	{
		LOG_ERROR_INFO("get sub segment error!\n");
		return -1;
	}

	//printf("****** auth_flag:%d, who:%d *******\n", auth_ip[who].authflag, who);
	pthread_mutex_lock(&auth_lock);
	if (auth_ip[who].authflag == 1)
		goto _return_auth_;
	if (auth_ip[who].authflag == 0)
	{
		//printf("url: %s%s\n", host, uri);

		if (isStrnexit(uri, uri_len, AUTH_SECCESS_FLAG, AUTH_SECCESS_FLAG_LEN))
		{
			//认证成功了
			get_peer_mac(phonefd, auth_ip[who].mac);
			auth_ip[who].authflag = 1;
			auth_ip[who].start = GetSystemTime_Sec();
			auth_ip[who].validTime = AUTHEN_INTERVAL;
			mangle_accept_rule(get_rule_num(who), who, "-R");
			save_auth_conf(who);
			LOG_NORMAL_INFO("%d : auth success...\n", who);
			//跳转到商家主页 ---- 应该是让后台认证服务器再认证OK了 让手机跳转的
			//repalce_with_pin(uri, uri_len, AUTH_SECCESS_FLAG, AUTH_SECCESS_FLAG_LEN, "?a=b", 4);
			//char successUrl[1024];
			//sprintf(successUrl, "http://%s/%s", host, uri);
			//web_302_jump(phonefd, successUrl);
			goto _return_auth_;
		}
		
		if (isStrnexit(uri, uri_len, AUTH_JUMP_WEB_PAGE_FLAG, AUTH_JUMP_WEB_PAGE_FLAG_LEN))
		{
			//跳转成功了,只能说手机已经执行了跳转操作了，但是是否显示出页面还是要手机以及使用者习惯
			char mac[32] = {0};
			get_peer_mac(phonefd, mac);
			char respons[128];
			sprintf(respons, "HTTP/1.1 200 OK\r\n"
							 "Content-Length: %d\r\n"
							 "\r\n"
							 "%s", strlen(mac), mac);
			if (socket_send(phonefd, respons, strlen(respons), 15) > 0)
			{
				auth_ip[who].jumpflag = 1;
				auth_ip[who].start = GetSystemTime_Sec();
				auth_ip[who].validTime = AUTHEN_JUMP_INTERVAL;
				LOG_NORMAL_INFO("jump web page is ok . %d\n", who);
			}
			goto _return_auth_failt_;
		}

		//判断是否是微信扫码发起的链接
		if (check_from_wethat(header, header_len))
		{
			char portalUrl[512];
			sprintf(portalUrl, "%s?authUrl=%s&extend=null", AUTHEN_URL, AUTHEN_URL_FOR_WECHAT);
			web_302_jump(phonefd, portalUrl);
			goto _return_auth_failt_;
		}

		/*苹果放行，因为在连接wifi后，如果没有放行，那么取消按键不会变成完成*/
		//必须先发送了web
		if (auth_ip[who].jumpflag
		&& isStrnexit(host, host_len, "captive.apple.com", 17))
		{
			//<HTML><HEAD><TITLE>Success</TITLE></HEAD><BODY>Success</BODY></HTML>
			//printf("<HTML><HEAD><TITLE>Success</TITLE></HEAD><BODY>Success</BODY></HTML>\n");
			//auth_ip[who].jumpflag = 0;
			goto _return_auth_;
		}
		
		if (strcmp(host, AUTHEN_HOST)
		&& (! isStrnexit(host, host_len, "weixin", 6))) //微信放行
		{
			//正常认证跳转
			web_302_jump(phonefd, AUTHEN_URL);
			goto _return_auth_failt_;
		}
	}
_return_auth_:
	pthread_mutex_unlock(&auth_lock);
	return 1;	
_return_auth_failt_:	
	pthread_mutex_unlock(&auth_lock);
	return 0;
}
#endif
