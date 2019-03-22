#include "includes.h"

char *exist_field_contentlength(char *header, int headerlen)
{
	char *p = strstr(header, "\r\nContent-Length:");
	if (p)
	{
		p += strlen("\r\nContent-Length:");
	}

	return p;
}

void struct_http_header(char * method, char * path, char * host, int port, char *data, char * header_buff)
{
	if (strcmp("GET", method) == 0)
	{
		sprintf(header_buff, "%s %s HTTP/1.1\r\n"
							 "Host: %s:%d\r\n"
							 "\r\n", 
							 method, path, host, port);
	}
	else if (strcmp("POST", method) == 0)
	{
		int content_length = 0;
		if (data)
		{
			content_length = strlen(data);
		}
		sprintf(header_buff, "%s %s HTTP/1.1\r\n"
							 "Host: %s:%d\r\n"
							 "Content-Length: %d\r\n"
							 "Content-Type:application/x-www-form-urlencoded\r\n"
							 "\r\n"
							 "%s",
							 method, path, host, port, content_length, data);
	}
	else
	{
		LOG_ERROR_INFO("can't know http method!\n");
	}
}

bool getRequestParma(char *data, char *parma_name, char *value, int value_size)
{
	char *p = straddr(data, parma_name);
	if (p)
	{
		p += strlen(parma_name);
		if (*p == '=')
		{
			p ++;

			int i, j = 0;
			for (i = 0; i < strlen(p); i ++)
			{
				if (p[i] == ' '
				|| p[i] == '\r'
				|| p[i] == '\n'
				|| p[i] == '&')
				{
					break;
				}
				
				value[j ++] = p[i];
				value[j] = 0;

				if (j > value_size - 1)
					break;
			}
			return true;
		}
	}

	return false;
}

int url_add_param(char *param_name, char *param_arg, char *addr)
{
	int len, bufsize;
	char *pjs = addr;
	len = strlen(param_name);
	memcpy(pjs, param_name, len);
	pjs += len;
	len = strlen(param_arg);
	bufsize = len * 4 + 1;
	char code[bufsize];
	len = url_encode((unsigned char *)param_arg, len, (unsigned char *)code, bufsize);
	memcpy(pjs, code, len);
	pjs += len;
	return pjs - addr;
}

int web_302_jump(int client_sock, char * to_url)
{
	char header_buff[4048] = {0};
	char *http = "http://";
	char *p = straddr(to_url, "http://");
	if (p)
	{
		p = to_url + 7;
	}
	else
	{
		p = straddr(to_url, "https://");
		if (p)
		{
			p = to_url + 8;
			http = "https://";
		}

		if (! p)
		{
			p = to_url;
		}
	}
	
#if 0
	int Year = 0, Mon= 0, Data = 0, Hour = 0, Min = 0, Sec = 0, Weekday = 0;
	GetAllSystemTime(&Year, &Mon, &Data, &Hour, &Min, &Sec, &Weekday);
	sprintf(header_buff, 
			"HTTP/1.1 302 Found\r\n"
			"Date: %s, %d %s 20%d %d:%d:%d GMT\r\n"
			"Content-Type: text/html;charset=utf-8\r\n"
			"Content-Length: 0\r\n"
			"Connection: Close\r\n"
			"Location: %s\r\n"
			"Server: apache\r\n"
			"\r\n",
			weekday(Weekday), Data, month(Mon), Year, Hour, Min, Sec, 
			to_url	);
#endif
	sprintf(header_buff, 
			"HTTP/1.0 302 Moved Temporarily\r\n"
			"Location: %s%s\r\n"
			"Content-Type: text/html; \r\n"
			"Content-Length: 0\r\n\r\n", 
			http, p	);

  	LOG_NORMAL_INFO("%s", header_buff);
    int len = strlen(header_buff);
    if (socket_send(client_sock, header_buff, len, 10) != len)
    {
    	LOG_PERROR_INFO("send error.");
        return -1;
    }
    return 0;
}

int get_content_length(char *header, int header_len)
{
	char *p = exist_field_contentlength(header, header_len);
	if (p)
	{
		char length[15];
		int i = 0, j = 0;
		for (i = 0; ; i ++)
		{
			if (p[i] == ' ')
				continue;
			if (p[i] == '\r'
			|| p[i] == '\n'
			|| p[i] == '\0')
			{
				break;
			}
			length[j ++] = p[i];
			if (j >= 14)
				return -1;
		}
		length[j] = 0;
		return atoi(length);
	}

	return -1;
}

int read_method(int phonefd, char *buf)
{
	int ret = -1;
	int pos = 0;
	for ( ; ; )
	{
		ret = socket_read(phonefd, buf + pos, 1, 5);
		if (ret <= 0)
		{
			LOG_ERROR_INFO("read method error.\n");
			return -1;
		}
		if (buf[pos] == ' ')
		{
			if (pos)
			{
				buf[pos] = '\0';
				return pos;
			}
			LOG_ERROR_INFO("read method error, only read space!\n");
			return -1;
		}
		pos += ret;
		if (pos > 15)
		{
			LOG_ERROR_INFO("read method error, method length too long!\n");
			return -1;
		}
	}
	
	return -1;
}

int read_url(int phonefd, char *buf)
{
	int ret = -1;
	int pos = 0;
	for ( ; ; )
	{
		ret = socket_read(phonefd, buf + pos, 1, 5);
		if (ret <= 0)
		{
			LOG_ERROR_INFO("read url error.\n");
			return -1;
		}
		if (buf[pos] == ' ')
		{
			if (pos)
			{
				buf[pos] = '\0';
				return pos;
			}
			LOG_ERROR_INFO("read url error, only read space!\n");
			return -1;
		}
		pos += ret;
		if (pos >= MAXBUF)
		{
			LOG_ERROR_INFO("read url error, method length too long!\n");
			return -1;
		}
	}
	return -1;
}

eMethod_t httpmethod_cmp(char *method)
{
	if (0 == strcmp("GET", method))
	{
		return E_GET;
	}
	if (0 == strcmp("POST", method))
	{
		return E_POST;
	}
	return E_UNKOWN;
}

int get_http_host(char *header, int header_len, char *buf, int bufsize, int *port)
{
	int offset = 0;
	char *p = strnaddr_case(header, header_len, "\r\nHost:", 7);
	if (!p)
	{
		//p = strnaddr(header, header_len, "\r\nhost:", 7);
		//if (!p)
		{
			//这个是处理腾讯微信发送图片消息的变态处理，腾讯这个变态
			//用的是X-Online-Host:，真是个王八蛋
			p = strnaddr_case(header, header_len, "\r\nX-Online-Host:", 16);
			if (!p)
			{
				LOG_ERROR_INFO("can't find host!\n");
				return -1;
			}
			else
				offset = 16;
		}
		//else
		//{
			//offset = 7;
		//}
	}
	else
	{
		offset = 7;
	}
	p += offset;
	
	int i, j = 0, portflag = 0;
	for (i = 0; ; i ++)
	{
		if (p[i] == ' ')
			continue;
		if (p[i] == '\r'
		|| p[i] == '\n' 
		|| p[i] == '\0')
		{
			break;
		}
		if (p[i] == ':')
		{
			portflag = 1;
			break;
		}
		buf[j ++] = p[i];
		if (j >= bufsize)
		{
			LOG_ERROR_INFO("len too long!\n");
			return -1;
		}
	}
	buf[j] = '\0';
	
	if (port)
	{
		if (portflag)
		{
			char httpport[10];
			int k = 0;
			for (++ i; ; i ++)
			{
				if (p[i] == ' ' 
				|| p[i] == '\r'
				|| p[i] == '\n' 
				|| p[i] == '\0')
				{
					break;
				}
				httpport[k ++] = p[i];
				if (k >= 10)
				{
					return -1;
				}
			}
			httpport[k]= '\0';
			*port = atoi(httpport);
		}
		else
		{
			*port = 80;
		}
	}
	return j;
}

int get_http_uri(char *header, int header_len, char *buf, int bufsize)
{
	char *p = strnaddr(header, header_len, " ", 1);
	if (!p)
	{
		return -1;
	}
	
	char *line = strnaddr(p+1, (header + header_len - p - 1), "\r\n", 2);
	if (!line)
	{
		return -1;
	}
	
	char *p1 = strnaddr(p+1, (header + header_len - p - 1), " ", 1);
	if (!p1)
	{
		return -1;
	}

	if (p1 >= line)
	{
		buf[0] = '/';
		buf[1] = '0';
		return 1;
	}

	int len = p1 - p - 1;
	if (len > bufsize)
	{
		return -1;
	}
	if (len == 0)
	{
		buf[0] = '/';
		buf[1] = '0';
		return 1;		
	}
	memcpy(buf, p+1, len);
	buf[len] = '\0';
	
	return len;
}

int analysis_host_uri_port(char *header, int header_len, char *host, int host_size, char *uri, int uri_size)
{
	int serverport = 80;

	int host_len = get_http_host(header, header_len, host, host_size, &serverport);
	if (host_len <= 0)
	{
		LOG_ERROR_INFO("get http host error!\n");
		return -1;
	}

	int uri_len = get_http_uri(header, header_len, uri, uri_size);
	if (uri_len <= 0)
	{
		LOG_ERROR_INFO("get http uri error!\n");
		return -1;		
	}

	return serverport;
}



