#include "includes.h"

void evhttpd_redirect_login(struct evhttp_request *req, const t_http_server *http_server)
{
	char redirect_url[HEADER_COOKIE_LEN + 1] = {0};
	snprintf(redirect_url, HEADER_COOKIE_LEN, "http://%s:%d%s", 
			http_server->gw, http_server->gw_http_port, WEB_LOGIN_PATH);
	evhttp_gw_reply_302_redirect(req, redirect_url);
}

void evhttpd_login_finish(struct evhttp_request *req, const t_http_server *http_server)
{
	char redirect_url[HEADER_COOKIE_LEN + 1] = {0};

	char cookie[HEADER_COOKIE_LEN];
	memset(cookie, 0, HEADER_COOKIE_LEN);
	create_cookie(cookie);
	snprintf(redirect_url, HEADER_COOKIE_LEN, "http://%s:%d%s", 
			http_server->gw, http_server->gw_http_port, WEB_HOME_PATH);
	LOG_HL_INFO("REDIRECT: %s\n", redirect_url);
	LOG_HL_INFO("cookie: %s\n", cookie);
	evhttp_gw_reply_302_redirect_with_cookie(req, redirect_url, cookie);
}

void evhttpd_webtalk(struct evhttp_request *req, void *arg)
{
	//struct evhttp_connection *con = evhttp_request_get_connection(req);

	t_http_server *http_server = (t_http_server *)arg;

	/**
	*进行密码验证:
	**/
	switch (evhttpd_filter_password(req))
	{
		case E_PASSWD_OK:
		{
			/**
			*	密码验证成功，就立即设置session，然后跳转到首页
			*	未验证成功的，进入session验证
			**/
			evhttpd_login_finish(req, http_server); return;
		}
		case E_PASSWD_INVALID:
		{
			break;
		}
		case E_NOT_PASSWD_LOGIN:
		default :
		{
			switch (evhttpd_filter_session(req))
			{
				case E_SESSION_OK:
				{
					switch (evhttp_request_get_command(req))
					{
						case EVHTTP_REQ_GET: evhttpd_get(req, http_server); return;
						case EVHTTP_REQ_POST: evhttpd_post(req, http_server); return;
						default : LOG_NORMAL_INFO("unkown http method!\n"); break;
					}
					
					break;
				}
				case E_SESSION_INVALID:
				default :
				{
					LOG_ERROR_INFO("E_SESSION_INVALID \n");
					break;
				}
			}
		}
	}

	evhttpd_redirect_login(req, http_server);
	
	//if (con) evhttp_connection_free(con);
}

int evhttp_server(const t_http_server *http_server)
{
	struct event_base *base;
	struct evhttp *http;
	struct evhttp_bound_socket *handle;

	base = event_base_new();
	if (! base)
	{
		LOG_PERROR_INFO("event_base_new failt !\n");
		goto end_loop;
	}

	/* Create a new evhttp object to handle requests. */
	http = evhttp_new(base);
	if (! http)
	{
		goto end_loop;
	}

	evhttp_set_gencb(http, evhttpd_webtalk, (void *)http_server);

	handle = evhttp_bind_socket_with_handle(http, http_server->gw, http_server->gw_http_port);
	if (!handle)
	{
		goto end_loop;
	}

	event_base_dispatch(base);

end_loop:
	if (handle)
		evhttp_del_accept_socket(http, handle);

	if (http)
		evhttp_free(http);

	if (base)
		event_base_free(base);

	return -1;
}

void *_create_auth_server_(void *arg)
{
	pthread_detach(pthread_self());

	t_http_server http_server;

	char cmd[255];
	EXECUTE_CMD(cmd, "rm -rf /tmp/www;sync;tar zxvf /etc/app/www.gz -C /tmp/;sync");
	//EXECUTE_CMD(cmd, "rm -rf /tmp/www;sync;tar zxvf /tmp/www.gz -C /tmp/;sync");

	struct timeval tCurr;
	gettimeofday(&tCurr, NULL);
	srand((unsigned)tCurr.tv_usec);
	
	for ( ; ; )
	{
		//初始化cookie
		s_cookie_init();

		memset(&http_server, 0, sizeof(t_http_server));
		http_server.gw_http_port = WEB_SERVER_PORT;
		http_server.base_path = WEB_DOC_BASE;
		get_dev_ip(http_server.gw, LAN_DEV);
		evhttp_server(&http_server);

		sleep(1);
	}

	return NULL;
}

int evhttpd_create_server_thread()
{
	pthread_t thd;
	if (pthread_create(&thd, NULL, _create_auth_server_, NULL) != 0)
	{
		LOG_PERROR_INFO("pthread_create failt!");
		return -1;
	}

	return 0;
}



