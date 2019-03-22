#if 0
#include "includes.h"


#define MAXBUF	512

char *exist_field_contentlength(char *header, int headerlen)
{
	char *p = straddr(header, "\r\nContent-Length:");
	if (p)
	{
		p += strlen("\r\nContent-Length:");
	}

	return p;
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
	while (1)
	{
		ret = socket_read(phonefd, buf + pos, 1, 3);
		if (ret <= 0)
		{
			LOG_PERROR_INFO("read method error.");
			return -1;
		}
		if (buf[pos] == ' ')
		{
			if (pos)
			{
				buf[pos] = '\0';
				return pos;
			}
			LOG_NORMAL_INFO("read method error, only read space!\n");
			return -1;
		}
		pos += ret;
		if (pos > 15)
		{
			LOG_NORMAL_INFO("read method error, method length too long!\n");
			return -1;
		}
	}
	return -1;
}

int read_url(int phonefd, char *buf)
{
	int ret = -1;
	int pos = 0;
	while (1)
	{
		ret = socket_read(phonefd, buf + pos, 1, 10);
		if (ret <= 0)
		{
			LOG_PERROR_INFO("read url error.");
			return -1;
		}
		if (buf[pos] == ' ')
		{
			if (pos)
			{
				buf[pos] = '\0';
				return pos;
			}
			LOG_NORMAL_INFO("read url error, only read space!\n");
			return -1;
		}
		pos += ret;
		if (pos >= MAXBUF)
		{
			LOG_NORMAL_INFO("read url error, method length too long!\n");
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

static bool get_header_path(char *header_line, int len, char *path, int bufsize)
{
	int i, j = 0;
	for (i = 0; i < len; i ++)
	{
		if (header_line[i] == ' ' || header_line[i] == '\r' || header_line[i] == '\n')
		{
			break;
		}
		path[j ++] = header_line[i];
		path[j] = 0;
		if (j >= bufsize)
		{
			return false;
		}
	}

	return true;
}

static bool get_header_cookie(char *header_line, int len, char *cookie, int bufsize)
{
	char *p = straddr(header_line, "Cookie: ");
	if (p && p == header_line)
	{
		p = p + strlen("Cookie: ");

		int i, j = 0;
		for (i = 0; i < strlen(p); i ++) //i = 1开始，跳过一个空格
		{
			if (p[i] == ' ' || p[i] == '\r' || p[i] == '\n')
			{
				break;
			}
			cookie[j ++] = p[i];
			cookie[j] = 0;

			if (j >= bufsize)
			{
				return false;
			}
		}
	}
	else
	{
		return false;
	}
	
	return true;
}

static int get_header_contenlen(char *header_line, int len)
{
	int content_len = -1;
	char *p = straddr(header_line, "Content-Length: ");
	if (p && p == header_line)
	{
		p = p + strlen("Content-Length: ");

		int i, j = 0;
		char tmp[15] = {0};
		for (i = 0; i < strlen(p); i ++) //i = 1开始，跳过一个空格
		{
			if (p[i] == ' ' || p[i] == '\r' || p[i] == '\n')
			{
				break;
			}
			
			tmp[j ++] = p[i];
			tmp[j] = 0;

			if (j >= 15)
			{
				return -1;
			}
		}
		content_len = atoi(tmp);
	}
	else
	{
		return -1;
	}
	
	return content_len;
}

bool analysis_header(int phonefd, char *path, int path_bufsize, 
	char *host, int host_bufsize, char *cookie, int cookie_bufsize, int *content_len)
{
	int content_len_tmp = 0;
	bool path_flg = false;
	char header_line[MAXBUF+1];
	while (1)
	{
		int ret = readline(phonefd, header_line, MAXBUF);
		if (ret <= 0)
		{
			return false;
		}

		header_line[ret]= 0;

		LOG_WARN_INFO("%s", header_line);

		if (header_line[ret - 1] != '\n')
		{
			LOG_ERROR_INFO("invalid header !\n");
			return false;
		}

		if (ret == MAXBUF && header_line[ret - 1] != '\n')
		{
			//不合法,qchttpd通信头部不会有行长度超过MAXBUF的
			LOG_ERROR_INFO("invalid header !\n");
			return false;
		}
		
		//获取path
		if (! path_flg)
		{
			path_flg = true;
			if (path)
			{
				get_header_path(header_line, ret, path, path_bufsize);
			}
		}

		//获取host
		if (host)
		{
			//nothing
		}
		
		//获取cookies
		if (cookie)
		{
			get_header_cookie(header_line, ret, cookie, cookie_bufsize);
		}

		//获取请求的正文长度,针对POST方法
		if (content_len && (content_len_tmp = get_header_contenlen(header_line, ret)) > 0)
		{
			*content_len = content_len_tmp;
		}
		
		if (strcmp(header_line, "\r\n") == 0)
		{
			break;
		}
	}

	return true;
}

int analysis_get(int phonefd)
{
	char path[HEADER_PATH_LEN]  = {0};
	char cookie[HEADER_COOKIE_LEN] = {0};
	if (! analysis_header(phonefd, path, HEADER_PATH_LEN, NULL, 0, cookie, HEADER_COOKIE_LEN, NULL))
	{
		return -1;
	}

	LOG_NORMAL_INFO("path: %s.\n", path);
	LOG_NORMAL_INFO("cookie: %s.\n", cookie);
	
	return http_response_process(phonefd, path, cookie, NULL);
}

int analysis_post(int phonefd)
{
	char path[HEADER_PATH_LEN + 1]  = {0};
	char cookie[HEADER_COOKIE_LEN + 1] = {0};
	int content_len = 0;
	if (! analysis_header(phonefd, path, HEADER_PATH_LEN, NULL, 0, cookie, HEADER_COOKIE_LEN, &content_len))
	{
		return -1;
	}

	//LOG_NORMAL_INFO("path: %s.\n", path);
	//LOG_NORMAL_INFO("cookie: %s.\n", cookie);
	//LOG_NORMAL_INFO("content_len: %d\n", content_len);

	char post_data[POST_CONTENT_LEN + 1] = {0};
	if (content_len > 0)
	{
		if (socket_read(phonefd, post_data, content_len, 5) != content_len)
		{
			LOG_ERROR_INFO("read post content error!\n");
			return -1;
		}
	}
	
	LOG_WARN_INFO("post_data: %s\n", post_data);
	
	return http_response_process(phonefd, path, cookie, post_data);
}
#endif

