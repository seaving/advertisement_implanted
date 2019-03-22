#include "includes.h"

typedef struct sockaddr SA;

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

int connect_noblock(int clientfd, struct sockaddr_in *serveraddr)
{
	if (connect(clientfd, (SA *)serveraddr, sizeof(struct sockaddr_in)) >= 0) {
		//connect success
		return 0;
	}
#ifdef DEBUG_SOCKET	
	LOG_WARN_INFO("(s:%d)connect need to check. e:%d, %d, %d %s\r\n", 
					clientfd, errno, ENETUNREACH, EINPROGRESS, strerror(errno));
#endif

	// 不是正在连接，表示是其它错误，则应该是网络原因导致CONNECT失败，则直接返回错误
	// 网络不可到达，表示网络有问题
	// ENETUNREACH : 101
	// EINPROGRESS : 115
	if ((errno != EINPROGRESS) || (errno == ENETUNREACH))
	{
		//网络不可达，网络有问题
		return -1;
	}

	// 检测是否有数据
	fd_set socketRed;
	fd_set socketWr;
	struct timeval sTimeout;
	int i = 0, ret = -1;
    socklen_t len;
    int iError = -1;
    
	while (1)
	{
		// 实际测试中，直接关机后启动的第一次连接，一般会超时，然后再连接的时候，会很快连接上(下面打印的计数次数为7)
		i ++;
		if (i > 20)
		{
#ifdef DEBUG_SOCKET
			LOG_WARN_INFO("(s:%d) connect time out.\r\n", clientfd);
#endif
			//超时退出
			return -1;
		}
#ifdef DEBUG_SOCKET
		LOG_NORMAL_INFO("(s:%d) connect ...........................%d\r\n", clientfd, i);
#endif

		// 用select来判断是否connect成功
		// 初始化
		FD_ZERO(&socketRed);
		FD_ZERO(&socketWr);
		FD_SET (clientfd, &socketRed);
		FD_SET (clientfd, &socketWr);

		// 设定延时时间
		sTimeout.tv_sec  = 1;
		sTimeout.tv_usec = 0;

		// select
		ret = select(clientfd + 1, (fd_set*)(&socketRed), (fd_set*)(&socketWr), (fd_set*)NULL, &sTimeout);
		if (ret <= 0)
		{
			if (ret == 0)
			{
				//select超时
				continue;
			}
			if (ret < 0)
			{
				//select出错
				return -1;
			}
		}
		// 套接字可读、可写
		// 需要进一步判断
		if ((FD_ISSET(clientfd, &socketRed) > 0)
		 && (FD_ISSET(clientfd, &socketWr) > 0))
		{
#ifdef DEBUG_SOCKET		
			LOG_WARN_INFO("socket : %d read & write, need to check agin.\r\n", clientfd);
#endif
			len = sizeof(iError);
			ret = getsockopt(clientfd, SOL_SOCKET, SO_ERROR, &iError, &len);
			if (ret < 0)
			{
#ifdef DEBUG_SOCKET	
				LOG_ERROR_INFO("Get SO_ERROR error, connect failt.\r\n");
#endif
				// 获取SO_ERROR属性，也可能失败
				return -1;
			}
#ifdef DEBUG_SOCKET
			LOG_ERROR_INFO("Get ERROR parma:%d, %d\r\n", iError, ret);
#endif

			if (iError != 0)
			{
#ifdef DEBUG_SOCKET
				LOG_ERROR_INFO("ERROR != 0; connect failt.\r\n");
#endif
				// 表示链接到现在还没有建立
				// return -1;

				continue;
			}

			if (0 == iError)
			{
#ifdef DEBUG_SOCKET			
				LOG_NORMAL_INFO("ERROR = 0; connect success.\r\n");
#endif
				// 表示链接已经建立
				return 0;
			}
		}

		// socket 可写不可读，表示链接建立
		if ((FD_ISSET(clientfd, &socketRed) <= 0)
		 && (FD_ISSET(clientfd, &socketWr) > 0))
		{
#ifdef DEBUG_SOCKET
			LOG_NORMAL_INFO("socket:%d write & noread, connect success.\r\n", clientfd);
#endif
			return 0;
		}

		// socket 可读不可写，不知道怎么处理，先这么处理看看
		if ((FD_ISSET(clientfd, &socketRed) > 0)
		 && (FD_ISSET(clientfd, &socketWr) <= 0))
		{
#ifdef DEBUG_SOCKET
			LOG_ERROR_INFO("socket:%d read & nowrite, connect failt.\r\n", clientfd);
#endif
			// TCP socket 被设为非阻塞后调用 connect ，connect 函数如果没有马上成功，
			// 会立即返回 EINPROCESS（如果被中断返回EINTR） ，但 TCP 的 3 次握手继续进行。
			// 之后可以用 select 检查连接是否建立成功（但不能再次调用connect，这样会返回
			// 错误EADDRINUSE）,此处建议返回错误，走重新初始化流程(在实际测试中如果直接
			// 返回1或是continue都有可能导致程序的退出).
			return -1;
		}
	}
	
	return -1;
}

int open_clientfd_noblock(char *bindip, char *hostname, int port)
{
    int clientfd;
    struct sockaddr_in serveraddr;
	struct sockaddr_in local_addr;
	
    if ((clientfd = socket(AF_INET, SOCK_STREAM, 0)) < 0)
    {
		return -1;
	}

    char ipaddr[32] = {0};
    int ret = get_addr_info(hostname, ipaddr);
    if (ret == -1)
    {
    	close(clientfd);
		return -2;
	}
	else if (ret == -2)
	{
		close(clientfd);
		return -5;
	}
	
	struct linger linger;
	linger.l_onoff = 0;
	int optlen = sizeof(linger);
	setsockopt(clientfd, SOL_SOCKET, SO_LINGER, (char *)&linger, optlen);

	socket_setfd_noblock(clientfd);

	bzero((char *) &serveraddr, sizeof(serveraddr));
	serveraddr.sin_family = AF_INET;
	serveraddr.sin_addr.s_addr = inet_addr(ipaddr);
	serveraddr.sin_port = htons(port);
	
	if (bindip)
	{
		bzero((char *) &local_addr, sizeof(local_addr));
		local_addr.sin_family = AF_INET;
		local_addr.sin_addr.s_addr = inet_addr(bindip);
		local_addr.sin_port = 0;
		if (bind(clientfd, (struct sockaddr*)&local_addr, sizeof(local_addr)) != 0)
		{
			LOG_PERROR_INFO("bind error");
			close(clientfd);
			return -1;
		}
	}

	if (connect_noblock(clientfd, &serveraddr) < 0)
	{		
		close(clientfd);
		return -3;
	}
	
	socket_setfd_block(clientfd);

    return clientfd;
}

int socket_read(int sockfd, char *buf, int len)
{
	return read(sockfd, buf, len);
}

int socket_send(int sockfd, char *buf, int len)
{
	return send(sockfd, buf, len, 0);
}

int readline(int fd, void *usrbuf, int maxlen)
{
    int n, rc = 0;
    char c[2] = {0}, *bufp = (char *)usrbuf;

    n = 0;
    if ((maxlen - 1) <= 0)
    {
    	return -1;
	}
	
    while (n < maxlen - 1)
    {
		if ((rc = read(fd, c, 1)) == 1)
		{
			n ++;
			*bufp ++ = c[0];
			if (c[0] == '\n')
			{
				break;
			}
		}
		else
		{
			break;
		}
    }
    *bufp = 0;

    return n;
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

