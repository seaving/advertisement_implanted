#include "includes.h"
#if 0


#else

int g_success;
typedef struct
{
	struct evhttp_request *req;
	struct evhttp_uri *decoded;
	const char *path;
	char *decoded_path;
	char *whole_path;
	const char *uri;
	struct evbuffer *evb;
} auth_arg_t;

char auth(const char *path, const char *uri, char *portal, int *temp_accept_time)
{
	if (strcmp((char *)path, "/qcdog/auth") == 0)
	{
		char token[33] = {0};
		if (getRequestParma((char *)uri, "jumpweb", token, 32))
		{
			if (getRequestParma((char *)uri, "temp_accept", token, 32))
				*temp_accept_time = atoi(token);
			if (*temp_accept_time <= 0 || *temp_accept_time > 60)
				*temp_accept_time = TEMP_ACCEPT_TIME;
			
			return AUTH_SNIFF;
		}

		char id[33] = {0};
		if (! getRequestParma((char *)uri, "id", id, 32))
		{
			return AUTH_ERROR;
		}

		if (! getRequestParma((char *)uri, "token", token, 32))
		{
			return AUTH_ERROR;
		}
		LOG_HL_INFO("id: %s\ntoken : %s\n", id, token);

		char buf[2049] = {0};
		//char parm[64] = {0};
		//sprintf(parm, "token=%s", token);
		char path_tmp[256] = {0};
		snprintf(path_tmp, 255, AUTH_AUTH, id, token);
		struct_http_header("GET", path_tmp, AUTH_SERVER_HOST, AUTH_SERVER_PORT, NULL, buf);
		int sfd = connecthost(AUTH_SERVER_HOST, AUTH_SERVER_PORT, E_CONNECT_NOBLOCK);
		if (sfd <= 0)
		{
			return AUTH_ERROR;
		}
		LOG_HL_INFO(">>> %s\n", buf);
		socket_setfd_noblock(sfd);
		if (socket_send(sfd, buf, strlen(buf), 5) < 0)
		{
			socket_close(sfd);
			return AUTH_ERROR;
		}
		int ret = socket_read(sfd, buf, 2048, 5);
		if (ret <= 0)
		{
			socket_close(sfd);
			return AUTH_ERROR;
		}
		buf[ret] = '\0';
		
		socket_close(sfd);
		char *p = strnaddr(buf, ret, "Auth: ", 6);
		if (! p)
		{
			return -1;
		}

		p = p + 6;
		LOG_NORMAL_INFO("Auth: %c\n", *p);

		if (portal)
		{
			memset(portal, 0, 256);
			if (getRequestParma(buf, "portal", portal, 255))
			{
				LOG_NORMAL_INFO("portal: %s\n", portal);
			}
		}

		return *p;
	}
	else if (strcmp((char *)uri, "/qcdog/redirected") == 0)
	{
		return AUTH_REDIRECTED;
	}

	return AUTH_ERROR;
}

void fw_auth_allow(struct evhttp_request *req, int vaild_time, bool flag)
{
	char *peer_addr;
	ev_uint16_t peer_port;
	struct evhttp_connection *con = evhttp_request_get_connection(req);
	evhttp_connection_get_peer(con, &peer_addr, &peer_port);

	char *mac = arp_get(peer_addr);
	if (! mac)
		return ;

	auth_allow(peer_addr, mac ? mac : "ff:ff:ff:ff:ff:ff", vaild_time, flag);
	if (mac)
		free(mac);
}

static set_res_t _is_auth_client_set(struct evhttp_request *req)
{
	char *peer_addr;
	ev_uint16_t peer_port;
	struct evhttp_connection *con = evhttp_request_get_connection(req);
	evhttp_connection_get_peer(con, &peer_addr, &peer_port);

	char *mac = arp_get(peer_addr);
	if (! mac)
		return E_NOT_FOUND;

	return is_auth_client_set(peer_addr, mac);
}

void *_auth_work_(void *arg)
{
	if (! arg)
		return NULL;

	auth_arg_t *auth_arg = (auth_arg_t *)arg;

	struct evbuffer *evb = auth_arg->evb;
	struct evhttp_uri *decoded = auth_arg->decoded;
	const char *path = auth_arg->path;
	char *decoded_path = auth_arg->decoded_path;
	char *whole_path = auth_arg->whole_path;
	struct evhttp_request *req = auth_arg->req;
	const char *uri = auth_arg->uri;

	int temp_accept_time = TEMP_ACCEPT_TIME;
	char portal[256] = {0};
	switch (auth(path, uri, portal, &temp_accept_time))
	{
		case AUTH_ERROR:
		{
			break;
		}
		case AUTH_ALLOW:
		{	
			fw_auth_allow(req, AUTH_VAILD_TIME, FORMA_ACCEPT);

			if (strlen(portal) > 0)
			{
				printf("goto portal page : %s\n", portal);
				evhttp_gw_reply_js_redirect(req, portal);
			}
			else
			{
				evhttp_reply_response_connection_closed(req, 200, "OK", "SUCCESS", "欢迎使用前辰无线，您已认证成功！");
			}
			
			g_success = 1;
			goto done;
		}
		case AUTH_REDIRECTED:
		{
			LOG_ERROR_INFO("auth_redirected fail!\r\n");
			break;
		}
		case AUTH_SNIFF:
		{
			//判断是否已经放行,如果是正式放行就跳过
			if (_is_auth_client_set(req) != E_SETED)
			{
				LOG_NORMAL_INFO("---------------------------sniff init!\r\n");
				fw_auth_allow(req, /*temp_accept_time*/TEMP_ACCEPT_TIME, TMP_ACCEPT);
			}
			
			evhttp_reply_response_connection_closed(req, 200, "OK", NULL, NULL);

			goto done;
		}
	}

	evhttp_reply_response_connection_closed(req, 404, "Not Found", "authenticate failt !", "认证失败，请重新认证！");
	
done:
	if (decoded)
		evhttp_uri_free(decoded);
	if (decoded_path)
		free(decoded_path);
	if (whole_path)
		free(whole_path);
	if (evb)
		evbuffer_free(evb);
	if (arg)
		free(arg);

	return NULL;
}

void *_auth_thread_(void *arg)
{
	pthread_detach(pthread_self());
	return _auth_work_(arg);
}

static void auth_cb(struct evhttp_request *req, void *arg)
{
	struct evbuffer *evb = NULL;
	//const struct event_base *evbase = (struct event_base *)arg;
	const char *uri = evhttp_request_get_uri(req);
	struct evhttp_uri *decoded = NULL;
	const char *path;
	char *decoded_path;
	char *whole_path = NULL;
	
	if (evhttp_request_get_command(req) != EVHTTP_REQ_GET)
	{
		// only support HTTP GET
		goto err;
	}

	/* Decode the URI */
	decoded = evhttp_uri_parse(uri);
	if (! decoded)
	{
		evhttp_send_error(req, HTTP_BADREQUEST, 0);
		goto err;
	}

	LOG_NORMAL_INFO("uri: %s\n", evhttp_uri_get_query(decoded));

	/* Let's see what path the user asked for. */
	path = evhttp_uri_get_path(decoded);
	if (! path) path = "/";

	/* We need to decode it, to see what path the user really wanted. */
	decoded_path = evhttp_uridecode(path, 0, NULL);
	if (decoded_path == NULL)
		goto err;
	
	LOG_NORMAL_INFO("path: %s\n", path);

	auth_arg_t *auth_arg = calloc(sizeof(auth_arg_t), 1);
	if (! auth_arg)
		goto err;

	auth_arg->decoded = decoded;
	auth_arg->decoded_path = decoded_path;
	auth_arg->evb = evb;
	auth_arg->req = req;
	auth_arg->whole_path = whole_path;
	auth_arg->uri = uri;
	auth_arg->path = path;

	if (tpool_add_work(_auth_work_, (void *)auth_arg) < 0)
	{
		pthread_t thd;
		if (pthread_create(&thd, NULL, _auth_thread_, (void *)auth_arg) != 0)
		{
			LOG_PERROR_INFO("pthread_create error.");
			goto err;
		}
	}
	
	return ;
	
err:
	evhttp_send_error(req, 404, "authenticate failt !");

	if (decoded)
		evhttp_uri_free(decoded);
	if (decoded_path)
		free(decoded_path);
	if (whole_path)
		free(whole_path);
	if (evb)
		evbuffer_free(evb);
	if (auth_arg)
		free(auth_arg);
}

#endif

static int auth_server(const char *address, const t_http_server *http_server)
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

	evhttp_set_cb(http, "/qcdog/auth", auth_cb, (void *)base);
	//evhttp_set_gencb(http, auth_cb, (void *)http_server);

	handle = evhttp_bind_socket_with_handle(http, address, http_server->gw_http_port);
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

	for ( ; ; )
	{
		http_server.gw_http_port = QC_DOG_AUTH_SERVER_PORT;
		http_server.base_path = "/etc";

		char lan_ip[33] = {0};
		get_dev_ip(lan_ip, GET_LAN_NAME);
		auth_server(lan_ip, &http_server);

		sleep(1);
	}

	return NULL;
}



