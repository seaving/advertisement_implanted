#include "includes.h"

int socket_setfd_noblock(int sockfd)
{
    int flag = fcntl(sockfd, F_GETFL, 0);
    if (flag < 0)
    {
		LOG_PERROR_INFO("fcntl F_GETFL error");
		return -1;
    }
	if (fcntl(sockfd, F_SETFL, (flag | O_NONBLOCK)) < 0)
	{
		LOG_PERROR_INFO("fcntl F_SETFL error");
		return -1;
	}
	
	return 0;
}

int socket_setfd_block(int sockfd)
{
    int flag = fcntl(sockfd, F_GETFL, 0);
    if (flag < 0)
    {
		LOG_PERROR_INFO("fcntl F_GETFL error");
		return -1;
    }
	if (fcntl(sockfd, F_SETFL, (flag & ~O_NONBLOCK)) < 0)
	{
		LOG_PERROR_INFO("fcntl F_SETFL error");
		return -1;
	}
	
	return 0;
}

int socket_listen(char *bindip, int listen_port, int listen_counts)
{
    struct sockaddr_in server_addr;
    int listenfd;
    int ret, opt = 1;
    
    //初始化服务器地址
    bzero(&server_addr, sizeof(server_addr));
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(listen_port);
    if (bindip == NULL)
    {
    	server_addr.sin_addr.s_addr = htonl(INADDR_ANY);
    }
    else
    {
		server_addr.sin_addr.s_addr = inet_addr(bindip);
    }
    
    //创建监听套接字
    listenfd = socket(AF_INET, SOCK_STREAM, 0);
    if (listenfd < 0)
    {
        LOG_PERROR_INFO("Create socket error");
        return -1;
    }
    //设置地址可重用
    ret = setsockopt(listenfd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt));
    if (ret < 0)
    {
        LOG_PERROR_INFO("setsockopt error");
        return -1;
    }
    //绑定地址
    int len = sizeof(struct sockaddr);
    ret = bind(listenfd, (struct sockaddr*)&server_addr, len);
    if (ret < 0)
    {
        LOG_PERROR_INFO("bind error");
        return -1;
    }
    //开始监听,设置最大连接请求
    ret = listen(listenfd, listen_counts);
    if (ret < 0)
    {
        LOG_PERROR_INFO("listen error");
        return -1;
    }
    
    return listenfd;
}

int socket_setarg(int socket)
{
    //TCP_CORK：采用Nagle算法把较小的包组装为更大的帧
    int on = 1;
    setsockopt(socket, SOL_TCP, TCP_CORK, &on, sizeof(on));
    
    //设置非阻塞
    return socket_setfd_noblock(socket);
}

int socket_close_client(int sockfd)
{
	int sock = sockfd;
	if (sock > 0)
	{
		close(sock);
		
		//全局连接数减一
		g_connect_count --;

		LOG_HL_INFO("colse client fd: %d . g_connect_count = %d .\n", sock, g_connect_count);
	}
	return 0;
}

int socket_close(int sockfd)
{
	int sock = sockfd;
	if (sock > 0)
	{
		LOG_HL_INFO("close socket fd: %d .\n", sockfd);
		close(sock);
	}
	
	return 0;	
}

#if 1
int socket_read(int sock, char *buf, int readlen, socketstate_t *state)
{
    int count = 0;
    while (1)
    {
		count = read(sock, buf, readlen);
		if (count < 0)
		{
			if (errno == EAGAIN || errno == EWOULDBLOCK)
			{
				//读空了
				*state = E_EAGIN;
				PRINTF("socket_read: read E_EAGIN.\n");
				return 0;
			}
			else if (errno == EINTR)
			{
				// 被信号中断
				*state = E_EINTR;
				PRINTF("socket_read: read E_EINTR.\n");
				continue;
			}
			else
			{
				LOG_PERROR_INFO("fd: %d read error", sock);
				*state = E_ERROR;
				return -1;
			}
		}
		else if (count == 0)
		{
			// 被客户端关闭连接
			LOG_PERROR_INFO("fd: %d foreiner closed the connection. ", sock);
			*state = E_ERROR;
			return -1;
		}
		else
		{
			//printf("count = %d\n", count);
			*state = E_EAGIN;
			return count;
		}
    }

	*state = E_ERROR;
    return -1;
}

#else
int socket_read(int sock, char *buf, int readlen, socketstate_t *state)
{
	int n = 0;
	int nread = 0;
	while ((nread = read(sock, buf + n, readlen)) > 0)
	{
		n += nread;
		readlen -= nread;
		if (readlen <= 0)
			break;
	}
	if (nread == -1 && errno != EAGAIN)
	{
		LOG_PERROR_INFO("fd: %d read error. ", sock);
		*state = E_ERROR;
	}
	else
		*state = E_EAGIN;
		
	return n;
}
#endif

#if 1
int socket_send(int sock, char *buf, int len)
{
	ssize_t counts = 0;
	size_t total = len;
	size_t sended_len = 0;
	const char *p = buf;
	while(1)
	{
		counts = send(sock, p, total, MSG_NOSIGNAL);
		if (counts < 0)
		{
			if (errno == EAGAIN || errno == EWOULDBLOCK)
			{
				// EAGAIN 表示缓冲区为满，不能写入,稍加延时,在写
				usleep(1000);
				continue;
			}
			else if (errno == EINTR)
			{
				// 被信号中断,这里不进行写，直接退出
				return -1;
			}
			
			LOG_PERROR_INFO("[fd = %d] send error.", sock);
			return -1;
		}
		else if (counts == 0)
		{
			// 被客户端关闭连接
			LOG_PERROR_INFO("fd: %d send error, counts == 0. ", sock);
			return -1;
		}
		else
		{
			total -= counts;
			sended_len += counts;
			p += counts;
			if (total <= 0)
			{
				return sended_len;
			}
		}
	}
	
	return 0;
}
#else
int socket_send(int sock, char *buf, int len)
{
	int nwrite, data_size = len;  
	int n = 0;	
	while (data_size > 0)
	{  
		nwrite = write(sock, buf + n, data_size);  
		if (nwrite < n)
		{  
			if (nwrite == -1 && errno != EAGAIN)
			{
				LOG_PERROR_INFO("fd:%d, write error.", sock);
				return n;
			}  
			continue;
		}
		n += nwrite;
		data_size -= nwrite;
		if (data_size <= 0)
			return n;
	}

	return n;
}
#endif

