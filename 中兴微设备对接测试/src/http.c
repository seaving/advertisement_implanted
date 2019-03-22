

#include "includes.h"


void struct_http_header(char * method, char * path, char * host, char * header_buff)
{
	sprintf(header_buff, "%s %s HTTP/1.1\r\n"
						 "Host: %s\r\n"
						 "\r\n", 
						 method, path, host);
}

int get_url_path(char * url, char * host, char * path, int * port)
{
	int i, j = 0;
	int url_len = strlen(url);
	int offset = find_str(url, url_len, "http://", strlen("http://"));
	if (offset >= 0)
	{
		offset += strlen("http://");
	}
	else
	{
		offset = 0;
	}
	*port = 80;
	j = 0;
	for (i = offset; i < url_len; i ++)
	{
		if (url[i] == '/' || url[i] == ':') {
			break;
		}
		host[j ++] = url[i];
	}
	host[j] = 0;

	if (url[i] == ':')
	{
		char porttmp[20];
		j = 0;
		for (++ i; i < url_len; i ++)
		{
			if (url[i] == '/') {
				break;
			}
			porttmp[j ++] = url[i];
			if (j >= 20)
			{
				break;
			}
		}
		porttmp[j] = 0;
		if (j < 20 && port)
		{
			*port = atoi(porttmp);
		}
	}

	j = 0;
	for ( ; i < url_len; i ++)
	{
		path[j ++] = url[i];
	}
	if (j == 0)
	{
		path[j ++] = '/';
	}
	path[j] = 0;

	return 0;
}

int read_header(int fd, char * buffer, int buffer_len, int *read_end, socketstate_t *state)
{  
	int total_read = 0;
	int read_len = buffer_len;
	if (read_end)
	{
		*read_end = 0;
	}
	char *line_buffer = buffer;
    while (read_len > 0)
    {
		if (read_len > MAXLINE)
		{
			read_len = MAXLINE;
		}
        int ret = readline(fd, line_buffer, read_len, state);
        if (ret <= 0)
        {
            break;
        }
        total_read += ret;
        read_len = buffer_len - total_read;
		
        //读到了空行，http头结束
        if (0 < ret && ret <= 2)
        {
	        if (strcmp(line_buffer, "\r\n") == 0)
	        {
				if (read_end)
				{
					*read_end = 1;
				}
	        	break;
	        }
        }
        
		line_buffer += ret;
    }
    if (total_read <= 0)
    {
		return -1;
    }
    
    buffer[total_read] = '\0';

    return total_read;
}

int web_html_jump(int client_sock, char * to_url)
{
	char header_buff[4048] = {0};
	int content_len = 0;
	int Year, Mon, Data, Hour, Min, Sec, Weekday;
	char html[2548];
	memset(html, 0, 2548);
	sprintf(html, 	"<!DOCTYPE HTML>\r\n"
					"<html>\r\n"
					"<head>\r\n"
					"<meta http-equiv=\"Content-Language\" content=\"zh-CN\">\r\n"
					"<meta HTTP-EQUIV=\"Content-Type\" CONTENT=\"text/html; charset=gb2312\">\r\n"
					"<meta http-equiv=\"refresh\" content=\"0.1;url=%s\">\r\n"
					"<title></title>\r\n"
					"</head>\r\n"
					"<body>\r\n"
					"</body>\r\n"
					"</html>\r\n", to_url);

	content_len = strlen(html);
	
	GetAllSystemTime(&Year, &Mon, &Data, &Hour, &Min, &Sec, &Weekday);
	sprintf(header_buff, 
			"HTTP/1.1 200 OK\r\n"
			"Server: Tengine\r\n"
			"Date: %s, %d %s 20%d %d:%d:%d GMT\r\n"
			"Content-Length: %d\r\n"
			"Content-Type: text/html\r\n"
			"Connection: close\r\n"
			"\r\n"
			"%s", 
			weekday(Weekday), Data, month(Mon), Year, Hour, Min, Sec, 
			content_len, 
			html	);

  	LOG_NORMAL_INFO("%s", header_buff);
    int len = strlen(header_buff);
    if (socket_send(client_sock, header_buff, len) != len)
    {
        return -1;
    }
    return 0;
}

int web_302_jump(int client_sock, char * to_url)
{

	char header_buff[4048] = {0};
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

  	LOG_NORMAL_INFO("%s", header_buff);
    int len = strlen(header_buff);
    if (socket_send(client_sock, header_buff, len) != len)
    {
        return -1;
    }
    return 0;
}

int get_http_method(char *header, char *buf, int bufsize)
{
	int i, j = 0;
	for (i = 0; ; i ++)
	{
		if (header[i] == ' ' 
		|| header[i] == '\0')
		{
			break;
		}
		buf[j ++] = header[i];
		if (j >= bufsize)
		{
			break;
		}
	}
	buf[j] = '\0';
	
	return j;
}

int get_http_host(char *header, int header_len, char *buf, int bufsize, int *port)
{
	int offset = 0;
	char *p = strnaddr(header, header_len, "\r\nHost:", 7);
	if (!p)
	{
		p = strnaddr(header, header_len, "\r\nhost:", 7);
		if (!p)
		{
			//这个是处理腾讯微信发送图片消息的变态处理，腾讯这个变态
			//用的是X-Online-Host:，真是个王八蛋
			p = strnaddr(header, header_len, "\r\nX-Online-Host:", 16);
			if (!p)
				return -1;
			else
				offset = 16;
		}
		else
		{
			offset = 7;
		}
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

void change_header_encoding(char * header_buffer, int header_len)
{
	replace(header_buffer, header_len, "Accept-Encoding", 15, "Accept-Rubbish!", 15);
}

char * exist_field_contentlength(char *header, int headerlen)
{
	char *p = strnaddr(header, headerlen, "\r\nContent-Length:", 17);
	if (!p) p = strnaddr(header, headerlen, "\r\nContent-length:", 17);
	if (!p) p = strnaddr(header, headerlen, "\r\ncontent-length:", 17);
	//if (!p) p = NULL;
	return p;
}

char * exist_field_chunked(char *header, int headerlen)
{
	char *p = strnaddr(header, headerlen, "\r\nTransfer-Encoding:", 20);
	if (!p) p = strnaddr(header, headerlen, "\r\nTransfer-encoding:", 20);
	if (!p) p = strnaddr(header, headerlen, "\r\ntransfer-encoding:", 20);
	//if (!p) p = NULL;
	return p;
}

char * exist_field_location(char *header, int headerlen)
{
	char *p = strnaddr(header, headerlen, "\r\nLocation:", 11);
	//if (!p) p = strnaddr(header, headerlen, "\r\nlocation:", 11);
	//if (!p) p = NULL;
	return p;
}

int get_chunked_size(int socket, char *chunked, int chunkedbufsize, int *chunkedlen)
{
	char ch;
	int ret = -1;
	int i = 0, j = 0;
	int chunkedflag = 0;
	char chuned_size[15] = {0};
	socketstate_t state = E_ERROR;
	while (1)
	{
		READ_DATA(socket, &ch, 1, ret, state);
		chunked[i ++] = ch;
		if (i >= chunkedbufsize)
		{
			return -1;
		}
		if (ch == '\r' || ch == '\n')
		{
			chunkedflag ++;
			if (chunkedflag >= 2)
			{
				//chuned块长度获取完毕
				break;
			}
		}
		else
		{
			chunkedflag = 0;
			chuned_size[j ++] = ch;
			if (j >= 15)
			{
				return -1;
			}
		}
	}

	chunked[i] = '\0';
	
	if (i <= 0 || j <= 0)
	{
		return -1;
	}
	for (i = 0; i < j; i ++)
	{
		if (('0' <= chuned_size[i] && chuned_size[i] <= '9')
		|| ('a' <= chuned_size[i] && chuned_size[i] <= 'f'))
		{
			continue;
		}
		break;
	}
	if (i < j) {
		LOG_ERROR_INFO("extract chunked size error!!!(%s)\n", chuned_size);
		return -1;
	}
	*chunkedlen = strtol(chuned_size, NULL, 16);
	return i;
}

int get_content_length(char *header, int header_len)
{
	char *p = exist_field_contentlength(header, header_len);
	if (p)
	{
		p += 18;
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

int isHtml(char *header_buff, int header_len, char *uri, int urilen)
{
    int html_flag = 0;

	if (find_str(header_buff, header_len, "Accept-Encoding: gzip", 21) >= 0)
	{
		return 0;
	}
	if (find_str(header_buff, header_len, "Content-Encoding: gzip", 21) >= 0)
	{
		return 0;
	}

	if (isStrnexit(uri, urilen, ".js", 3)
	|| isStrnexit(uri, urilen, ".css", 4)
	|| isStrnexit(uri, urilen, ".json", 5))
	{
		return 0;
	}

	int contentype_len = 15;
	int ret = find_str(header_buff, header_len, "\r\nContent-Type:", contentype_len);
	if (ret < 0)
		ret = find_str(header_buff, header_len, "\r\nContent-type:", contentype_len);
	if (ret < 0)
		ret = find_str(header_buff, header_len, "\r\ncontent-type:", contentype_len);
	if (ret >= 0)
	{
		int len = header_len - (ret + contentype_len);
		char *pT = strnaddr(header_buff+ret+contentype_len, len, "\r\n", 2);
		if (pT)
		{
			int size = pT - header_buff - ret - contentype_len;
			if (0 < size)
			{
				char *pcontype;
				pcontype = header_buff+ret+contentype_len;
				char tmp[size + 1];
				memcpy(tmp, pcontype, size);
				tmp[size] = '\0';
				if (! straddr(tmp, "text") && ! straddr(tmp, "html"))
				{
					html_flag = 0;
				}
				else
				{
					html_flag = 1;
				}
			}
		}
	}

	return html_flag;
}



