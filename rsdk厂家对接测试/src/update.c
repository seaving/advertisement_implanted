#include "includes.h"

#define ID_SIZE			20

#define UPDATE_HOST		"121.196.215.235"
#define UPDATE_HOST_1	"http://" UPDATE_HOST
#define UPDATE_PORT		80

#define FILE_PATH		"/etc/app/mproxy_arm"

int md5(char *file, char *md5sum)
{
	FILE *fp = NULL;

	char cmd[255];
	sprintf(cmd, "md5sum %s | awk -F \" \" '{print $1}'", file);
	fp = popen(cmd, "r");
	if (NULL == fp)	
	{  
		LOG_PERROR_INFO("popen error!");  
		return -1;  
	}

	while(fgets(md5sum, 127, fp) != NULL)
	{
		break;
	}
	pclose(fp);

	int len = strlen(md5sum);
	if (len <= 0)
	{
		return -1;
	}
	
	int i;
	for (i = 0; i < len; i ++)
	{
		if (md5sum[i] == '\r' || md5sum[i] == '\n')
		{
			md5sum[i] = '\0';
			break;
		}
	}
	len = i;
	return len;
}

int check_result(char *res)
{
	LOG_NORMAL_INFO("%s\n", res);
	if (strcmp(res, "need't update") == 0)
	{
		return 0;
	}
	if (strcmp(res, "no unit in database") == 0)
	{
		return 0;
	}
	if (strcmp(res, "exception") == 0)
	{
		return -1;
	}
	if (strcmp(res, "token error") == 0)
	{
		return -1;
	}

	return 1;
}

int download(int sockfd)
{

	char buf[MAXBUF + 1];
	
	socketstate_t state = E_ERROR;
	int ret = read_header(sockfd, buf, MAXBUF, NULL, &state);
	if (ret <= 0)
	{
		LOG_ERROR_INFO("read update header error!\n");
		return -1;
	}
	LOG_NORMAL_INFO("%s", buf);


	char *p;
	char netMd5[128];
	
	//开始下载程序,写入到文件
	char *updatefile = "/etc/app/mproxy_arm.update";
	unlink(updatefile);

	ret = readline(sockfd, netMd5, 127, &state);
	if (ret <= 0)
	{
		return -1;
	}
	netMd5[ret] = '\0';
	if (ret <= 3)
	{
		if (strcmp(netMd5, "0\r\n") == 0)
		{
			return 0;
		}
		return 0;
	}	
	p = strchr(netMd5, '\r');
	if (p)
	{
		*p = '\0';
	}

	//判断结果
	if ((ret = check_result(netMd5)) <= 0)
	{
		return ret;
	}
	
	LOG_NORMAL_INFO("MD5SUM = %s\n", netMd5);

	//获取下载链接
	char download_url[256];
	ret = read(sockfd, download_url, 255);
	if (ret <= 0)
	{
		LOG_ERROR_INFO("can't get download url!\n");
		return -1;
	}

	download_url[ret] = 0;
	int download_url_len = ret;
	LOG_NORMAL_INFO("download_url: %s\n", download_url);
	//校验md5值
	char md5str[33] = {0};
	CalMd5(download_url, download_url_len, md5str);
	if (strcmp(md5str, netMd5))
	{
		LOG_ERROR_INFO("MD5 check error! netmd5:%s, urlmd5:%s\n", netMd5, md5str);
		return -1;
	}

	char dhost[256];
	char duri[256];
	int dport = 80;
	ret = get_url_path(download_url, dhost, duri, &dport);
	if (ret < 0)
	{
		LOG_ERROR_INFO("get_url_path ERROR!\n");
		return -1;
	}

	int downfd = connecthost(dhost, dport, E_CONNECT_BLOCK);
	if (downfd < 0)
	{
		return -1;
	}

	char header[MAXBUF];
	sprintf(header, "GET %s HTTP/1.1\r\nHost: %s\r\n\r\n", duri, dhost);
	LOG_NORMAL_INFO("%s", header);

	int header_len = strlen(header);
	ret = send(downfd, header, header_len, 0);
	if (ret != header_len)
	{
		LOG_PERROR_INFO("send download header error.");
		close(downfd);
		return -1;
	}

	ret = read_header(downfd, header, MAXBUF-1, NULL, &state);
	if (ret <= 0)
	{
		LOG_ERROR_INFO("read download respons error!\n");
		close(downfd);
		return -1;
	}
	header[ret] = 0;
	header_len = ret;

	int fd = open(updatefile, O_RDWR | O_CREAT | O_TRUNC);
	if (fd <= 0)
	{
		LOG_PERROR_INFO("open \"%s\" error!", updatefile);
		close(downfd);
		return -4;
	}

	int readed_len = 0;
	int writed_len = 0;
	while (1)
	{
		ret = socket_read(downfd, buf, MAXBUF, &state);
		if (ret <= 0)
		{
			close(downfd);
			close(fd);
			break;
		}
		buf[ret] = '\0';
		readed_len += ret;
		printf("update program len---> %d\n", readed_len);
		if (write(fd, buf, ret) != ret)
		{
			LOG_PERROR_INFO("write update file error!");
			close(downfd);
			close(fd);
			unlink(updatefile);
			return -4;
		}
		writed_len += ret;
	}
	close(downfd);
	close(fd);
	sync();

	unlink(FILE_PATH);
	char cmd[128];
	sprintf(cmd, "mv %s %s;chmod 777 %s;rm -rf %s;sync", updatefile, FILE_PATH, FILE_PATH, updatefile);
	system(cmd);
	return 1;
}

int RequestUpdate()
{
	char id[ID_SIZE];
	memset(id, 0, ID_SIZE);
	memcpy(id, ID, strlen(ID));
	int sockfd;
	int loop = 2;
	while (loop --)
	{
		sockfd = connecthost(UPDATE_HOST, UPDATE_PORT, E_CONNECT_NOBLOCK);
		if (sockfd <= 0)
		{
			LOG_ERROR_INFO("connect update server error!\n");
			return -1;
		}

		char parm[256];
		sprintf(parm, "?action=update&channelId=%s&routerMac=%s&platforms=%s&version=%s", 
						id, get_netdev_wan_mac(), PLATFORMS, VERSION);
		char tmp[256];
		sprintf(tmp, "%s%s", parm, TOKEN_KEY);

	 	unsigned char md5sum[16] = {0};
		myMd5((unsigned char *)tmp, strlen(tmp), (unsigned char *)md5sum);
		char md5str[33] = {0};
		sprintf(md5str, "%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x", 
				md5sum[0], md5sum[1], md5sum[2], md5sum[3], md5sum[4], md5sum[5], md5sum[6],
				md5sum[7], md5sum[8], md5sum[9], md5sum[10], md5sum[11], md5sum[12], md5sum[13],
				md5sum[14], md5sum[15]);	
		
		/**构造请求*/
		char http[MAXBUF];
		sprintf(http, 	"GET /plugin/api/updateUnit.jhtml%s&md5=%s HTTP/1.1\r\n"
						"Host: %s\r\n"
						"\r\n", 
						parm, md5str, UPDATE_HOST);
		LOG_NORMAL_INFO("%s", http);
		
		int send_len = strlen(http);
	 	if (socket_send(sockfd, http, send_len) != send_len)
	 	{
			LOG_ERROR_INFO("socket_send update server error!\n");
			close(sockfd);
			return -1;
	 	}

	 	int ret = download(sockfd);
		if (ret < 0)
		{
			//更新出错
			printf("[%s]update error!\n", MODULE_NAME);
		}
		else if (ret == 0)
		{
			//没有更新
			printf("[%s]no new high version program file to update.\n", MODULE_NAME);
			break;
		}
		else
		{
			//更新成功
			printf("[%s]update success.\n", MODULE_NAME);
			_exit_(-1);
		}
		close(sockfd);
		sleep(3);
	}
	close(sockfd);
	return 0;
}


