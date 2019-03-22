#include "includes.h"

#define USERNAME_LEN	17
#define PASSWORD_LEN	17

typedef struct _cookie_
{
	char sesson_name[12];
	char sesson_id[33];
	int validity;		//秒
	int start;		//秒
	bool flg;
	pthread_mutex_t lock;
	char username[USERNAME_LEN];
	char password[PASSWORD_LEN];
} cookie_t;

static cookie_t s_cookie = {
	.lock = PTHREAD_MUTEX_INITIALIZER
};

void s_cookie_init()
{
	memset(s_cookie.sesson_name, 0, 12);
	memset(s_cookie.sesson_id, 0, 33);
	s_cookie.start = 0;
	s_cookie.validity = 0;
	s_cookie.flg = false;
	memset(s_cookie.username, 0, USERNAME_LEN);
	memset(s_cookie.password, 0, PASSWORD_LEN);

	sprintf(s_cookie.username, "admin");
	snprintf(s_cookie.password, PASSWORD_LEN, "admin123");

	char password[32] = {0};
	if (get_passwd(password) > 0)
	{
		snprintf(s_cookie.password, PASSWORD_LEN, password);
	}
}

void s_cookie_set(char *sesson_name, char *session_id, int start, int validity, bool flg)
{
	memset(s_cookie.sesson_name, 0, 12);
	memcpy(s_cookie.sesson_name, sesson_name, strlen(sesson_name));
	memset(s_cookie.sesson_id, 0, 33);
	memcpy(s_cookie.sesson_id, session_id, strlen(session_id));
	s_cookie.start = start;
	s_cookie.validity = validity;
	s_cookie.flg = flg;
}

void s_cookie_password_set(char *password)
{
	snprintf(s_cookie.password, PASSWORD_LEN, password);
}

void create_cookie(char *cookie)
{
	char buf[64] = {0};
	struct timeval tCurr;
	gettimeofday(&tCurr, NULL);	
	sprintf(buf, "%d%d", (int)tCurr.tv_usec, rand());

	char md5[33] = {0};
	CalMd5ByCopy(buf, strlen(buf), X_AUTH_TOKEN_KEY, X_AUTH_TOKEN_KEY_LEN, md5);

	LOG_WARN_INFO("s_cookie_set\n");
	s_cookie_set("qcsessionid", md5, GetSystemTime_Sec(), SESSION_VALIDITY, true);

	sprintf(cookie, "%s=%s", "qcsessionid", md5);
}

bool cookie_check(char *cookie)
{
	bool ret = false;
	if (! cookie)
		return ret;

	if (strlen(cookie) <= 0)
		return ret;
	
	//判断cookie中的sessionid
	pthread_mutex_lock(&s_cookie.lock);
	if (s_cookie.flg && GetSystemTime_Sec() - s_cookie.start > s_cookie.validity)
	{
		LOG_NORMAL_INFO("sesson timeout!\n");
		
		//seesion超时
		s_cookie_init();
	}
	pthread_mutex_unlock(&s_cookie.lock);

	//cookie还在有效期
	if (s_cookie.flg)
	{
		//取出cookie中的字段sesson_name来
		char *p = straddr(cookie, s_cookie.sesson_name);
		if (p)
		{
			p += strlen(s_cookie.sesson_name);
			LOG_HL_INFO("*p: %c, s_cookie.sesson_name: %s\n", *p, s_cookie.sesson_name);
			if (*p == '=')
			{
				p += 1; //跳过 '='

				//获取32位数ID
				int i, j = 0;
				char session_id[33] = {0};
				for (i = 0; i < strlen(p); p ++)
				{
					if (p[i] == ' '
					|| p[i] == '\r'
					|| p[i] == '\n'
					|| p[i] == ';')
					{
						break;
					}

					session_id[j ++] = p[i];
					session_id[j] = 0;
					if (j >= 33)
						break;
				}
				
				LOG_HL_INFO("CMP: (c)%s , (s)%s\n", session_id, s_cookie.sesson_id);
				
				if (strcmp(session_id, s_cookie.sesson_id) == 0)
				{
					LOG_HL_INFO("cookie check success!\n");
					
					//重新更新session的有效期
					s_cookie.start = GetSystemTime_Sec();
					
					ret = true;
				}
				else
				{
					LOG_ERROR_INFO("cookie check failt!\n");
					s_cookie_init();
				}
			}
		}
	}

	return ret;
}
#if 0
http_filter_t login_check(char *path, char *context)
{
	//如果是其他页面，就直接跳入到cookie验证
	if (strcmp(path, WEB_LOGIN_CHECK_PATH))
		return E_HTTP_FILTER_NORMAL;
	
	if (! context)
	{
		//没有参数，跳转到login
		LOG_ERROR_INFO("login check failt!\n");
		return E_HTTP_FILTER_REDICRECT_LOGIN;
	}
	
	//转码
	char decode_context[strlen(context)+1];
	memset(decode_context, 0, sizeof(decode_context));
	
	url_decode(context, decode_context);
	
	//开始验证帐号和密码
	LOG_NORMAL_INFO("after url_decode: %s\n", decode_context);

	//获取username和password
	char username[USERNAME_LEN] = {0};
	char password[PASSWORD_LEN] = {0};
	getRequestParma(decode_context, "username", username, USERNAME_LEN - 1);
	getRequestParma(decode_context, "password", password, PASSWORD_LEN - 1);

	//验证
	if (strcmp(username, s_cookie.username) == 0
	&& strcmp(password, s_cookie.password) == 0)
	{
		//验证成功,跳转到home页面
		LOG_HL_INFO("login check success!\n");
		return E_HTTP_FILTER_REDICRECT_HOME;
	}
	
	//验证失败,跳转到login页面
	LOG_ERROR_INFO("login check failt!\n");
	return E_HTTP_FILTER_REDICRECT_LOGIN;
}
#endif

bool passwd_check(char *password)
{
	if (strlen(password) <= 0)
		return false;
	
	if (strlen(s_cookie.password) <= 0)
	{
		snprintf(s_cookie.password, PASSWORD_LEN, "%s", password);
	}

	//验证
	if (strcmp(password, s_cookie.password) == 0)
	{
		//验证成功,跳转到home页面
		LOG_HL_INFO("password check success!\n");
		return true;
	}
	
	//验证失败,跳转到login页面
	LOG_ERROR_INFO("password check failt!\n");
	return false;
}

#if 0
http_filter_t http_filter(char *path, char *cookie, char *post_data)
{
	if (path && strcmp(path, "/") == 0)
		return E_HTTP_FILTER_REDICRECT_LOGIN;
	
	//不是/html目录的放过
	if (path && strncmp(path, "/html/", strlen("/html/")))
		return E_HTTP_FILTER_NORMAL;

	//如果是设置密码页面就放过
	if (path && (strstr(path, WEB_SET_PASSWD)
	|| strstr(path, WEB_RESET_PASSWD)))
		return E_HTTP_FILTER_NORMAL;

	if (get_passwd(s_cookie.password) <= 0)
	{
		//跳入到密码设置页面
		LOG_ERROR_INFO("no set passwd, please set password!\n");
		return E_HTTP_FILTER_REDICRECT_SET_PASSWD;
	}

	if (! cookie || ! path)
		return E_HTTP_FILTER_ERROR;

	//如果是login来的请求，就直接验证帐户和密码
	http_filter_t ret = login_check(path, post_data);
	if (ret != E_HTTP_FILTER_NORMAL)
	{
		return ret;
	}

	//login页面直接放行而且不是html目录的也放行
	if (strcmp(path, WEB_LOGIN_PATH)
	 && (isStrexit(path, "html") || strcmp(path, "/") == 0))
	{
		LOG_ERROR_INFO("path: %s cookie check \n", path);
		
		//验证cookie
		if (! cookie_check(cookie))
		{
			//验证错误，跳转回到login页面
			return E_HTTP_FILTER_REDICRECT_LOGIN;
		}
	}
	
	//判断是否退出
	if (isStrexit(path, WEB_LOGIN_EXIT))
	{
		s_cookie_init();
		return E_HTTP_FILTER_REDICRECT_LOGIN;
	}
	
	//验证成功，继续流程
	return E_HTTP_FILTER_NORMAL;
}
#endif

