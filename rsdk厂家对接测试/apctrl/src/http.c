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

int get_chunked_size(int socket, char *chunked, int chunkedbufsize, int *chunkedlen)
{
	char ch;
	int ret = -1;
	int i = 0, j = 0;
	int chunkedflag = 0;
	char chuned_size[15] = {0};
	while (1)
	{
		ret = socket_read(socket, &ch, 1);
		if (ret <= 0)
			return -1;
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


char * exist_field_contentlength(char *header, int headerlen)
{
	char *p = strnaddr(header, headerlen, "\r\nContent-Length:", 17);
	if (!p) p = strnaddr(header, headerlen, "\r\nContent-length:", 17);
	if (!p) p = strnaddr(header, headerlen, "\r\ncontent-length:", 17);
	//if (!p) p = NULL;
	return p;
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


