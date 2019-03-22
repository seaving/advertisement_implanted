#include "includes.h"

#define WEBSERVER_CHECK_STATE(webserver, state, first...) \
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

int read_respons_header(webserverinfo_t *webserver)
{
	if (! webserver)
		return -1;
		
	int ret = -1;
	if (calloc_buf(webserver->respons, MAXBUF+1) < 0)
	{
		LOG_ERROR_INFO("calloc buf for respons error!\n");
		return -1;
	}

	socketstate_t state = E_ERROR;
	while (1)
	{
		char *buf = webserver->respons->buf + webserver->respons->datalen;
		int freesize = webserver->respons->bufsize - webserver->respons->datalen;
		if (freesize <= 0)
		{
			LOG_ERROR_INFO("freesize is <= 0! %d\n", freesize);
			return -1;
		}		
		ret = readline(webserver->serverfd, buf, freesize-1, &state);
		if (ret <= 0)
		{
			WEBSERVER_CHECK_STATE(webserver, state, "read webserver respons header error!\n");
		}
		buf[ret] = '\0';
		webserver->respons->datalen += ret;

		if (isStrnexit(webserver->respons->buf, webserver->respons->datalen, "\r\n\r\n", 4))
		{
			LOG_NORMAL_INFO(">>>>> %s", webserver->respons->buf);
		#if DEBUG_ON
			if (isStrnexit(webserver->respons->buf, webserver->respons->datalen, "HTTP/1.1 400 Bad Request", 24))
			{
				LOG_WARN_INFO("$$$$ bad request: %s%s\n", webserver->phone->request->host->buf, 
						webserver->phone->request->uri->buf);
			}
		#endif

			if (exist_field_chunked(webserver->respons->buf, webserver->respons->datalen))
			{
				webserver->tmp = 0;
				webserver->pagetype = E_CHUNKED;
			}
			else if ((webserver->tmp = get_content_length(webserver->respons->buf, webserver->respons->datalen)) >= 0)
			{
				webserver->pagetype = E_STATIC;
			}
			else
			{
				webserver->tmp = 0;
				webserver->pagetype = E_UNKOWN_PAGE;
			}
			
			if (exist_field_location(webserver->respons->buf, webserver->respons->datalen))
			{
				webserver->mitm = E_ACCEPT;
				if (webserver->pagetype != E_STATIC
				&& webserver->pagetype != E_CHUNKED)
				{
					ret = socket_send(webserver->phonefd, webserver->respons->buf, webserver->respons->datalen);
					if (ret != webserver->respons->datalen)
					{
						LOG_ERROR_INFO("send respons header to phone error! ret = %d\n", ret);
						return -1;
					}
					webserver->respons->datalen = 0;
					webserver->opt = E_WEBSERVER_FINISHED;
					return 0;
				}
			}
			
			//网页刷新
			if (isStrnexit(webserver->respons->buf, webserver->respons->datalen, "HTTP/1.1 304", 12)
			|| isStrnexit(webserver->respons->buf, webserver->respons->datalen, "HTTP/1.0 304", 12))
			{
				webserver->mitm = E_ACCEPT;
				if (webserver->pagetype != E_STATIC
				&& webserver->pagetype != E_CHUNKED)
				{
					ret = socket_send(webserver->phonefd, webserver->respons->buf, webserver->respons->datalen);
					if (ret != webserver->respons->datalen)
					{
						LOG_ERROR_INFO("send respons header to phone error! ret = %d\n", ret);
						return -1;
					}
					webserver->respons->datalen = 0;
					webserver->opt = E_WEBSERVER_FINISHED;
					return 0;
				}
			}
			else
			{
				if (webserver->mitm == E_ATTACK)
				{
					ret = isHtml(webserver->respons->buf, webserver->respons->datalen, 
						webserver->phone->request->uri->buf, webserver->phone->request->uri->datalen);
					if (ret == 0)
					{
						webserver->mitm = E_ACCEPT;
					}
				}
			}

			break;
		}
	}
	
	webserver->opt = E_READ_CONTEXT;
	return 0;
}

int analysis_context(webserverinfo_t *webserver)
{
	if (! webserver)
		return -1;
		
	int ret = -1;

	switch (webserver->pagetype)
	{
		case E_STATIC:
		{
			ret = process_static(webserver);
			break;
		}
		case E_CHUNKED:
		{
			ret = process_chunked(webserver);
			break;
		}
		default:
		{
			ret = default_forward(webserver);
			//printf(COLOR_YELLOW"======>>> %s"COLOR_NONE_C, webserver->respons->buf);
			//ret = -1;
			break;
		}
	}

	return ret;
}


int webserver_data_process(struct epoll_event event)
{
	webserverinfo_t *webserver = (webserverinfo_t *)event.data.ptr;
	if (! webserver)
		return -1;

	if (webserver->phone->opt != E_PHONE_FINISHED)
	{
		epoll_mod_to_webserver(webserver);
		return 0;
	}

#if __SOCKET_MONITOR__
	activity_socket_monitor(webserver->phone);
#endif
	
	int ret = -1;
	switch (webserver->opt)
	{
		case E_READ_RESPONES_HEADER:
		{
			ret = read_respons_header(webserver);
			if (webserver->opt == E_READ_CONTEXT)
				ret = analysis_context(webserver);
			break;
		}
		case E_READ_CONTEXT:
		{
			ret = analysis_context(webserver);
			break;
		}
		case E_WEBSERVER_FINISHED:
		{
			ret = 0;
			break;
		}
		default :
		{
			ret = -1;
			PRINTF(COLOR_RED "unkown webserver option ! ret = %d\n" COLOR_NONE_C, ret);
			break;
		}
	}

	if (ret < 0 || webserver->opt == E_WEBSERVER_FINISHED)
	{	
		epoll_del_sockfd(webserver->server_epollfd, webserver->serverfd);
		epoll_del_sockfd(webserver->phone_epollfd, webserver->phonefd);
		socket_close(webserver->serverfd);
		socket_close_client(webserver->phonefd);
#if __SOCKET_MONITOR__		
		free_phoneinfo_member(webserver->phone);
		free_webserver_member(webserver);
#else
		free_phoneinfo(webserver->phone);
		free_webserver(webserver);
#endif
	}
	else
	{
		epoll_mod_to_webserver(webserver);
	}
	return 0;
}

void *webserver_data_process_thread(void *arg)
{
	pthread_detach(pthread_self());
	
	webserver_data_process(*((struct epoll_event *)arg));
	free (arg);
	arg = NULL;

	return NULL;
}

void *callback_webserver_data_process(void *arg)
{	
	webserver_data_process(*((struct epoll_event *)arg));
	free (arg);
	arg = NULL;
	
	return NULL;
}

int create_webserver_data_process_thread(struct epoll_event event)
{
	struct epoll_event *arg = calloc(1, sizeof(struct epoll_event));
	if (! arg)
	{
		LOG_PERROR_INFO("calloc error.");
		return -1;
	}
	memcpy(arg, &event, sizeof(struct epoll_event));
	if (tpool_add_work(callback_webserver_data_process, arg) == -2)
	{
		pthread_t thip;
		if (pthread_create(&thip, NULL, webserver_data_process_thread, arg) != 0)
		{
			LOG_PERROR_INFO("pthread_create error.");
			if (arg)
				free (arg);
			arg = NULL;

			//重新启动程序，防止后面的人不能上网，达到均衡

			
			return -1;
		}
	}
	return 0;
}


