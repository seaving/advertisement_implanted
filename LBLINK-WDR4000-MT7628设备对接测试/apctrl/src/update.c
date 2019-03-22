#include "includes.h"

#define ID_SIZE			20

#define UPDATE_HOST		"120.77.149.125"
#define UPDATE_HOST_1	"http://" UPDATE_HOST
#define UPDATE_PORT		80

#define FILE_PATH		"/tmp/app"
#define UPDATE_PATH		"/tmp/app.tar"
#define VERSION_TEXT	"/tmp/app/version.txt"

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
	if (isStrexit(res, "need't update"))
	{
		return 0;
	}
	if (isStrexit(res, "no unit in database"))
	{
		return 0;
	}
	if (isStrexit(res, "exception"))
	{
		return -1;
	}
	if (isStrexit(res, "token error"))
	{
		return -1;
	}

	return 1;
}

int download(int sockfd)
{
	char buf[MAXBUF + 1] = {0};
	
	int ret = read_header(sockfd, buf, MAXBUF, NULL);
	if (ret <= 0)
	{
		LOG_ERROR_INFO("read update header error!\n");
		return -1;
	}
	LOG_NORMAL_INFO("%s", buf);

	//判断是否是chunked或者content-length
	int len = 0;
	int contentLen = get_content_length(buf, ret);
	memset(buf, 0, MAXBUF);
	if (contentLen < 0)
	{
		//按照动态来接收
		LOG_WARN_INFO("can't find content length, try chunked ... \n");
		len = readChunkedText(sockfd, buf, MAXBUF);
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
		len = readStaticText(sockfd, contentLen, buf);
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
	LOG_HL_INFO("%s\n", buf);

	char *p;
	char netMd5[128] = {0};
	
	//开始下载程序,写入到文件
	char *updatefile = UPDATE_PATH;
	unlink(updatefile);
	int offset = 0;
	ret = strline(buf, netMd5);
	if (ret <= 0)
	{
		return -1;
	}
	offset += ret;
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
	char download_url[1024] = {0};
	ret = strline(buf+offset, download_url);
	if (ret <= 0)
	{
		LOG_ERROR_INFO("can't get download url!\n");
		return -1;
	}
	int i;
	for (i = 0; i < ret; i ++)
	{
		if (download_url[i] == '\r'
		|| download_url[i] == '\n')
		{
			download_url[i] = '\0';
			break;
		}
	}
	download_url[ret] = 0;
	LOG_NORMAL_INFO("download_url: %s\n", download_url);

#if 1
	//计算连接MD5
	char durlMd5[33]= {0};
	CalMd5(download_url, strlen(download_url), durlMd5);
	if (strcmp(durlMd5, netMd5))
	{
		LOG_ERROR_INFO("download url md5 error. netMd5: %s, durlMd5: %s\n", netMd5, durlMd5);
		return -1;
	}
#endif

	char dhost[256];
	char duri[256];
	int dport = 80;
	ret = get_url_path(download_url, dhost, duri, &dport);
	if (ret < 0)
	{
		LOG_ERROR_INFO("get_url_path ERROR!\n");
		return -1;
	}

	int downfd = open_clientfd_noblock(NULL, dhost, dport);
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

	ret = read_header(downfd, header, MAXBUF-1, NULL);
	if (ret <= 0)
	{
		LOG_ERROR_INFO("read download respons error!\n");
		close(downfd);
		return -1;
	}
	header[ret] = 0;
	header_len = ret;

	LOG_NORMAL_INFO("download response: %s\n", header);

	contentLen = get_content_length(header, header_len);

	if (contentLen < 0)
	{
		LOG_WARN_INFO("can't find content length, try chunked ... \n");
		contentLen = readChunkedTextToFile(downfd, updatefile);
		if (contentLen <= 0)
		{
			close(downfd);
			return -1;
		}

	}
	else if (contentLen > 0)
	{
		contentLen = readStaticTextToFile(downfd, contentLen, updatefile);
		if (contentLen <= 0)
		{
			close(downfd);
			return -1;
		}
	}
	else
	{
		close(downfd);
		return -1;
	}
	
#if 0
	//校验文件
	char update_file_md5[33] = {0};
	md5(updatefile, update_file_md5);
	if (strcmp(update_file_md5, netMd5))
	{
		LOG_ERROR_INFO("MD5 check error! netmd5:%s, upfilemd5:%s\n", netMd5, update_file_md5);
		//close(downfd);
		//return -1;
	}
#endif

	close(downfd);
	return 1;
}

char version[32] = {0};
int get_version_from_txt(char *version)
{
	int fd = open(VERSION_TEXT, O_RDWR);
	if (fd <= 0)
	{
		LOG_PERROR_INFO("open %s error.", VERSION_TEXT);
		return -1;
	}
	if (read(fd, version, 20) <= 0)
	{
		sprintf(version, "GET_VERSION_ERROR");
		LOG_PERROR_INFO("read error.");
		close(fd);
		return -1;
	}
	close(fd);
	char *p = strchr(version, '\n');
	if (p)
		*p = '\0';
	p = strchr(version, '\r');
	if (p)
		*p = '\0';
	return 0;
}

int RequestUpdate()
{
	char id[ID_SIZE];
	memset(id, 0, ID_SIZE);
	memcpy(id, ID, strlen(ID));
	int sockfd;
	//int loop = 2;
	//while (loop --)
	{
		sockfd = open_clientfd_noblock(NULL, UPDATE_HOST, UPDATE_PORT);
		if (sockfd <= 0)
		{
			LOG_ERROR_INFO("connect update server error!\n");
			return -1;
		}

		if (strlen(version) <= 0)
		{
			sprintf(version, "0.0");
		}
		char parm[256];
		sprintf(parm, "?action=update&channelId=%s&routerMac=%s&platforms=%s&version=%s", 
						id, get_netdev_wan_mac(), PLATFORMS, version/*VERSION*/);
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
		sprintf(http, 	"GET /qc/api.html%s&md5=%s HTTP/1.1\r\n"
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
	 	close(sockfd);
		if (ret < 0)
		{
			//更新出错
			LOG_SAVE("update error!\n");
			return -1;
		}
		else if (ret == 0)
		{
			//没有更新
			LOG_SAVE("no new high version program file to update.\n");
			return 0;
		}
		else
		{
			//更新成功
			//LOG_SAVE("update success.\n");
			remove(FILE_PATH);
			//解压
			char tar_cmd[255];
			mkdir(FILE_PATH, 0777);
			sprintf(tar_cmd, "tar -xvf %s -C %s", UPDATE_PATH, FILE_PATH);
			system(tar_cmd);
			remove(UPDATE_PATH);
			get_version_from_txt(version);
			LOG_SAVE("update success version : %s\n", version);
			return 1;
		}
		sleep(3);
	}
	return 0;
}


