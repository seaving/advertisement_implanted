#include "includes.h"

int open_udp_client(char *ser_ip, int port, struct sockaddr_in *server)
{
	int fd = socket(AF_INET, SOCK_DGRAM, 0);
	if (fd < 0) 
	{
		LOG_PERROR_INFO("create socket error.");
		return -1;
	}
	
    int opt = -1;
	setsockopt(fd, SOL_SOCKET, SO_BROADCAST, (char*)&opt, sizeof(opt));

	memset(server, 0, sizeof(struct sockaddr_in));
	server->sin_family = AF_INET;
	server->sin_port = htons(port);
	server->sin_addr.s_addr = inet_addr(ser_ip);

	return fd;
}

int open_udp_client_s(char *ser_ip, int ser_port, struct sockaddr_in *server, int my_port)
{
	int fd = socket(AF_INET, SOCK_DGRAM, 0);
	if (fd < 0) 
	{
		LOG_PERROR_INFO("create socket error.");
		return -1;
	}
	
    int opt = -1;
	setsockopt(fd, SOL_SOCKET, SO_BROADCAST, (char*)&opt, sizeof(opt));

	memset(server, 0, sizeof(struct sockaddr_in));
	server->sin_family = AF_INET;
	server->sin_port = htons(ser_port);
	server->sin_addr.s_addr = inet_addr(ser_ip);

	struct sockaddr_in myaddr;
	bzero(&myaddr, sizeof(struct sockaddr_in));
	myaddr.sin_family = AF_INET;
	myaddr.sin_addr.s_addr = htonl(INADDR_ANY);
	myaddr.sin_port = htons(my_port);

	if (bind(fd, (struct sockaddr *)&myaddr, sizeof(struct sockaddr_in)) < 0)
	{
		close(fd);
		return -1;
	}

	return fd;
}

int open_udp_server(int port)
{
    int fd = socket(AF_INET, SOCK_DGRAM, 0);
    if (fd < 0)
    {
        LOG_PERROR_INFO("create socket error.");
        return -1;
    }
    
    int opt = -1;
	setsockopt(fd, SOL_SOCKET, SO_BROADCAST, (char*)&opt, sizeof(opt));

    struct sockaddr_in sa;
    memset(&sa, 0, sizeof(sa));
    sa.sin_family = AF_INET;
    sa.sin_port = htons(port);
    sa.sin_addr.s_addr = INADDR_ANY;

    int ret = bind(fd, (struct sockaddr *)&sa, sizeof(struct sockaddr_in));
    if (ret < 0)
    {
        LOG_PERROR_INFO("bind error.");
        return -1;
    }

	return fd;
}

int udp_recv(int fd, char *buf, int buf_size, struct sockaddr_in *from)
{
    socklen_t slen = sizeof(struct sockaddr_in);
    ssize_t size;
    memset(from, 0, slen);
    size = recvfrom(fd, buf, buf_size, 0, (struct sockaddr *)from, &slen);
    if (size <= 0)
    {
        LOG_PERROR_INFO("recvfrom error.");
        return -1;
    }

    LOG_NORMAL_INFO("recvfrom ip: %s, port: %d\n", inet_ntoa(from->sin_addr), ntohs(from->sin_port));

	return size;
}

int udp_select_recv(int fd, char *buf, int buf_size, struct sockaddr_in *from, int timeout_sec)
{
	struct timeval tv;
	fd_set readfds;
	
    socklen_t slen = sizeof(struct sockaddr_in);
    ssize_t size;
    memset(from, 0, slen);

	for ( ; ; )
	{
		FD_ZERO(&readfds);
		FD_SET(fd, &readfds);
		tv.tv_sec = timeout_sec;
		tv.tv_usec = 0;
		int ret = select(fd + 1, &readfds, NULL, NULL, &tv);
		if (ret == 0)
		{
			return 0;
		}
		else if (ret < 0)
		{
			LOG_PERROR_INFO("select error.");
			return -1;
		}
		else
		{
			if (FD_ISSET(fd, &readfds))
			{
				size = recvfrom(fd, buf, buf_size, 0, (struct sockaddr *)from, &slen);
				if (size <= 0)
				{
					LOG_PERROR_INFO("recvfrom error.");
					return -1;
				}

				LOG_NORMAL_INFO("recvfrom ip: %s, port: %d\n", inet_ntoa(from->sin_addr), ntohs(from->sin_port));

				return size;
			}
		}
	}
	
	return -1;
}

int udp_send(int fd, char *buf, int data_len, struct sockaddr_in *to)
{
    ssize_t size = sendto(fd, buf, data_len, 0, (struct sockaddr *)to, sizeof(struct sockaddr_in));
    if (size < 0)
    {
        LOG_PERROR_INFO("sendto error.");
        return -1;
    }

	return size;
}

int udp_send_to(int fd, char *buf, int data_len, char *to_ip, int to_port)
{
	struct sockaddr_in addrto;
	int nlen = sizeof(struct sockaddr_in);
	bzero(&addrto, nlen);
	addrto.sin_family = AF_INET;
	if (strcmp(to_ip, UDP_BROADCAST) == 0)
	{
		addrto.sin_addr.s_addr = htonl(INADDR_BROADCAST);
	}
	else
	{
		addrto.sin_addr.s_addr = inet_addr(to_ip);
	}

	addrto.sin_port = htons(to_port);

	return udp_send(fd, buf, data_len, &addrto);
}


