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

int readChunkedText(int socket, char *buf, int bufsize)
{
	int bufOffset = 0;
	char chunked[50];
	int freeBufSize = bufsize - bufOffset;
	while (1)
	{
		int offset = 0;
		memset(chunked, 0, 50);
		for ( ; ; )
		{
			char ch;
			int len = read(socket, &ch, 1);
			if (len != 1)
			{
				perror("read chunked size error.");
				return -1;
			}
			if (ch == '\r'
			|| ch == '\n')
				break;
			chunked[offset ++] = ch;
		}
		int contentLen = strtol(chunked, NULL, 16);
		LOG_NORMAL_INFO("[+] chunked size = %d\n", contentLen);
		if (contentLen == 0)
		{
			return bufOffset;
		}
		else if (contentLen < 0)
		{
			return -1;
		}
		char tmp[2];
		if (read(socket, tmp, 1) <= 0)
		{
			perror("read chunked flag error.");
			return -1;
		}
		if (contentLen > freeBufSize)
		{
			contentLen = freeBufSize;
		}
		int readlen = 0;
		while (contentLen > 0)
		{
			if (contentLen > MAXBUF)
			{
				readlen = MAXBUF;
			}
			else
			{
				readlen = contentLen;
			}
			int len = read(socket, buf + bufOffset, readlen);
			if (len <= 0)
			{
				perror("read chunked text error.");
				return -1;				
			}
			bufOffset += len;
			buf[bufOffset] = '\0';
			contentLen -= len;
		}
		freeBufSize = bufsize - bufOffset;
		if (freeBufSize <= 0)
			return bufOffset;
		if (read(socket, tmp, 2) <= 0)
		{
			perror("read chunked flag error.");
			return -1;
		}
	}
	return 0;
}

int readStaticText(int socket, int content_length, char *buf)
{

	int readed_len = 0;
	int readlen = 0;

	while (content_length > 0)
	{
		if (content_length > (MAXBUF - 1))
			readlen = MAXBUF - 1;
		else
			readlen = content_length;
		int ret = read(socket, buf + readed_len, readlen);
		if (ret <= 0)
		{
			return -1;
		}
		readed_len += ret;
		content_length -= ret;
		buf[readed_len] = 0;
	}

	return readed_len;
}

int readChunkedTextToFile(int socket, char *file)
{
	int readed_len = 0;
	char chunked[50];
	char buf[MAXBUF+1];
	int fd = open(file, O_RDWR | O_CREAT | O_TRUNC);
	if (fd <= 0)
	{
		LOG_PERROR_INFO("open \"%s\" error!", file);
		return -1;
	}
	
	while (1)
	{
		int offset = 0;
		memset(chunked, 0, 50);
		for ( ; ; )
		{
			char ch;
			int len = read(socket, &ch, 1);
			if (len != 1)
			{
				perror("read chunked size error.");
				goto _return_error_;
			}
			if (ch == '\r'
			|| ch == '\n')
				break;
			chunked[offset ++] = ch;
		}
		int contentLen = strtol(chunked, NULL, 16);
		LOG_NORMAL_INFO("[+] chunked size = %d\n", contentLen);
		if (contentLen == 0)
		{
			goto _return_;
		}
		else if (contentLen < 0)
		{
			goto _return_error_;
		}
		char tmp[2];
		if (read(socket, tmp, 1) <= 0)
		{
			LOG_PERROR_INFO("read chunked flag error.");
			goto _return_error_;
		}
		int readlen = 0;
		while (contentLen > 0)
		{
			if (contentLen > MAXBUF)
			{
				readlen = MAXBUF;
			}
			else
			{
				readlen = contentLen;
			}
			int len = read(socket, buf, readlen);
			if (len <= 0)
			{
				LOG_PERROR_INFO("read chunked text error.");
				goto _return_error_;				
			}
			buf[len] = '\0';
			contentLen -= len;
			readed_len += len;
			if (write(fd, buf, len) != len)
			{
				LOG_PERROR_INFO("write error.");
				goto _return_error_;
			}
		}
		if (read(socket, tmp, 2) <= 0)
		{
			LOG_PERROR_INFO("read chunked flag error.");
			goto _return_error_;
		}
	}
_return_:
	close(fd);
	sync();
	return readed_len;
_return_error_:
	close(fd);
	return -1;
}

int readStaticTextToFile(int socket, int content_length, char *file)
{
    int fd = open(file, O_CREAT | O_RDWR | O_TRUNC, S_IRUSR | S_IWUSR | S_IRGRP | S_IWGRP | S_IROTH);
    if(fd < 0)
    {
        LOG_PERROR_INFO("open %s error.", file);
        return -1;
    }

#if DEBUG_ON
	int old_down = 0;
	int totallen = content_length;
	int begin_time = GetSystemTime_Sec();
#endif

	int readlen = 0;
	int readed_len = 0;
	int len = 0;
	char buf[MAXBUF+1];
	while (content_length > 0)
	{
		if (content_length > MAXBUF)
			readlen = MAXBUF;
		else
			readlen = content_length;
		len = socket_read(socket, buf, readlen);
		if (len <= 0)
			break;
		buf[len] = '\0';
		readed_len += len;
		content_length -= len;
		if (write(fd, buf, len) != len)
		{
			LOG_PERROR_INFO("write error.");
			close(fd);
			return -1;
		}
#if DEBUG_ON
		int down = (int)(100 * ((float)((float)readed_len / (float)totallen)));
		float speed = 0.0;
		if (GetSystemTime_Sec() - begin_time > 0)
			speed = (float)((float)readed_len / (float)(GetSystemTime_Sec() - begin_time));
		if (down > old_down)
		{
			old_down = down;
			//system("clear");
			//for (i = 0; i < down; i ++)
				//PRINTF("#");
			PRINTF("download info: %.02f KB, %d%%, %.02f KB/s, total %.02f KB, free %.02f KB\n", 
					(float)((float)readed_len/(float)1024), down, speed/1024, 
					(float)((float)totallen/(float)1024), 
					(float)((float)(totallen - readed_len)/(float)1024));
		}
#endif
	}
	close(fd);
	if (content_length == 0)
	{
		LOG_HL_INFO("download success.\n");
		return readed_len;
	}
	LOG_PERROR_INFO("read error.");
	remove(file);
	return -1;
}

int http_add_param(char *param_name, char *param_arg, char *addr)
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


