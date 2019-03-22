#include "includes.h"

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

				if (j >= value_size - 1)
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

int read_header(int fd, char * buffer, int buffer_len, int *read_end)
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
        int ret = readline(fd, line_buffer, read_len);
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

int get_content_length(char *header, int header_len)
{
	char *p = straddr_case(header, "\r\nContent-Length:");
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
			int len = socket_read(socket, &ch, 1, 5);
			if (len != 1)
			{
				LOG_PERROR_INFO("read chunked size error.");
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
		if (socket_read(socket, tmp, 1, 5) <= 0)
		{
			LOG_PERROR_INFO("read chunked flag error.");
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
			int len = socket_read(socket, buf + bufOffset, readlen, 5);
			if (len <= 0)
			{
				LOG_PERROR_INFO("read chunked text error.");
				return -1;				
			}
			bufOffset += len;
			buf[bufOffset] = '\0';
			contentLen -= len;
		}
		freeBufSize = bufsize - bufOffset;
		if (freeBufSize <= 0)
			return bufOffset;
		if (socket_read(socket, tmp, 2, 5) <= 0)
		{
			LOG_PERROR_INFO("read chunked flag error.");
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
		int ret = socket_read(socket, buf + readed_len, readlen, 5);
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

int http_read_response_content(int socket, char *response, int size)
{
	char *buf = calloc(1, MAXBUF);
	if (buf == NULL)
		return -1;
	
	int ret = read_header(socket, buf, MAXBUF, NULL);
	if (ret <= 0)
	{
		LOG_ERROR_INFO("read update header error!\n");
		free_malloc(buf);
		return -1;
	}
	LOG_NORMAL_INFO(">>>>> %s", buf);

	ret = 0;

	//判断是否是chunked或者content-length
	int len = 0;
	int contentLen = get_content_length(buf, ret);
	free_malloc(buf);

	if (contentLen < 0)
	{
		//按照动态来接收
		LOG_WARN_INFO("can't find content length, try chunked ... \n");
		len = readChunkedText(socket, response, MAXBUF < size ? MAXBUF : size);
		if (len <= 0)
		{
			LOG_ERROR_INFO("readChunkedText failt!\n");
			return -1;
		}
	}
	else if (contentLen > 0)
	{
		if (contentLen > MAXBUF) {
			contentLen = MAXBUF;
		}
		len = readStaticText(socket, contentLen < size ? contentLen : size, response);
		if (len <= 0)
		{
			LOG_ERROR_INFO("readStaticText failt!\n");
			return -1;
		}
	}
	else
	{
		LOG_ERROR_INFO("content len == 0 !\n");
		return -1;
	}
	
	return 0;
}

int http_send_post_data(char *host, char *path, int port, char *data, char *content_type, char *response, int response_size)
{
	int socketfd = connecthost(host, port, E_CONNECT_NOBLOCK);
	if (socketfd < 0)
	{
		return -1;
	}

	int ret = -1;
	char *header = calloc(1, strlen(data) + 255);
	if (header)
	{
		char mac[33] = {0};
		get_local_mac(mac, get_netdev_wan_name());
		
		char token[33] = {0};
		CalMd52(data, strlen(data), "qianchen", path, X_AUTH_TOKEN_KEY, X_AUTH_TOKEN_KEY_LEN, token);

		snprintf(
			header, 512, 
			"POST %s HTTP/1.1\r\n"
			"Host: %s:%d\r\n"
			"Content-Type: %s\r\n"
			"Content-Length: %d\r\n"
			"x-auth: %s;%s;%s;%s\r\n"
			"\r\n"
			"%s",
			path, host, port, 
			content_type ? content_type : "application/json", 
			strlen(data), 
			CHANNEL, mac, "qianchen", token,
			data
		);

		LOG_WARN_INFO(">>>>>>>> %s\n", header);
		
		int header_len = strlen(header);
		ret = socket_send(socketfd, header, header_len, 10);
		free_malloc(header);
		
		if (ret != header_len)
		{
			ret = -1;
			LOG_ERROR_INFO("socket send size %d, src size: %d\n", ret, header_len);
		}
		else
		{
			ret = 0;
			
			if (response && response_size > 0)
			{
				ret = http_read_response_content(socketfd, response, response_size);
			}
		}
	}

	socket_close(socketfd);

	return ret;
}

