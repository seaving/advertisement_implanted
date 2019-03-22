#include "includes.h"

int insert_domain_conver(char * s_url, char * c_url)
{
	stlmap_hash_insert(s_url, c_url);
	return 0;
}

int init_domain_map()
{
	stlmap_hash_init();
	return 0;
}

char * get_conver_url(char * s_url)
{
	return stlmap_hash_get(s_url);
}

int del_domain_conver(char * s_url)
{
	stlmap_hash_delect(s_url);
	return 0;
}

#define MAX_DATA_BUFF_SIZE	8192 * 10
int request_domain_conver_list()
{	
	int i, k = 0, j = 0;
	int ret = 0;
	int socket;
	char request_host[250] = {0};
	char request_path[1250] = {0};
	char request_header[1500] = {0};
	int request_port = REQUEST_LINK_PORT;
	int src_len = strlen(REQUEST_LINK_URL);
	int offset = 0;
	char * request_link = REQUEST_LINK_URL;

	offset = find_str(request_link, src_len, "http://", strlen("http://"));
	if (offset >= 0)
	{
		offset += strlen("http://");
	}
	else
	{
		offset = 0;
	}

	j = 0;
	for (i = offset; i < src_len; i ++)
	{
		if (*(request_link + i) == '/')
		{
			break;
		}
		request_host[j ++] = *(request_link + i);
	}
	request_host[j] = 0;

	j = 0;
	for ( ; i < src_len; i ++)
	{
		request_path[j ++] = *(request_link + i);
	}

	socket = connecthost(request_host, request_port, E_CONNECT_NOBLOCK);
	if (socket < 0)
	{
		LOG_ERROR_INFO("request domain server error!!!\n");
		close(socket);
		return -1;
	}

	memset(request_header, 0, 1500);
	struct_http_header("GET", request_path, request_host, 80, NULL, request_header);
	int header_len = strlen(request_header);
	ret = socket_send(socket, request_header, header_len, 15);
	if (ret != header_len)
	{
		LOG_ERROR_INFO("send data \"%s\" error!!!\n", request_header);
		goto _return_;
	}

	int recv_len = 0;
	char recv_buff[MAX_DATA_BUFF_SIZE] = {0};
	char recv_buff_tmp[2048];
	char *p = recv_buff;

	socketstate_t state = E_ERROR;
	while (1)
	{
		ret = socket_read(socket, recv_buff_tmp, 2048, &state);
		if (ret <= 0)
		{
			break;
		}
		recv_len += ret;
		if (recv_len >= MAX_DATA_BUFF_SIZE - 2)
		{
			recv_len -= ret;
			break;
		}

		memcpy(p, recv_buff_tmp, ret);
		p += ret;
		*p = 0;
	}

	if (recv_len <= 0)
	{
		ret = -1;
		goto _return_;
	}

	LOG_HL_INFO("%s\n", recv_buff);
	offset = find_str(recv_buff, recv_len, "\r\n\r\n", strlen("\r\n\r\n"));
	if (offset < 0) {
		ret = -1;
		goto _return_;
	}

	offset += strlen("\r\n\r\n");
	char src_url[2048] = {0};
	char con_url[2048] = {0};
	int flag = 0;
	j = 0, k = 0;
	for (i = offset; i < recv_len; i ++)
	{
		if (recv_buff[i] == '\n')
		{
			flag = 1;
			i ++;
		}
		if (recv_buff[i] == '\n')
		{
			flag = 0;
			i ++;

			LOG_HL_INFO("%s --> %s\n", src_url, con_url);
			insert_domain_conver(src_url, con_url);
			memset(src_url, 0, 2048);
			memset(con_url, 0, 2048);
			j = 0, k = 0;
		}
		
		if (flag == 0)
		{
			if (recv_buff[i] != '\r' && recv_buff[i] != '\n')
			{
				src_url[j ++] = recv_buff[i];
			}
		}
		else {
			if (recv_buff[i] != '\r' && recv_buff[i] != '\n')
			{
				con_url[k ++] = recv_buff[i];
			}
		}
	}
	
_return_:
	close(socket);
	return ret;
}


