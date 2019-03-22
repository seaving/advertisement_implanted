#include "includes.h"

int g_channelNameLen = 0;
char g_channelName[256] = {0};

int RequestChannelName()
{
	char buf[MAXBUF+1] = {0};
	char header[MAXBUF+1] = {0};
	sprintf(buf, "?action=getChannel&routerMac=%s", get_netdev_wan_mac());
	char md5str[33] = {0};
	CalMd5(buf, strlen(buf), md5str);
	sprintf(header, "GET /qc/api.html%s&md5=%s HTTP/1.1\r\n"
					"Host: %s\r\n"
					"\r\n",
					buf, md5str, QC_HOST);
	LOG_NORMAL_INFO("%s", header);
	int sockfd = open_clientfd_noblock(NULL, QC_HOST, QC_PORT);
	if (sockfd <= 0)
	{
		LOG_ERROR_INFO("connect to %s:%d failt!\n", QC_HOST, QC_PORT);
		return -1;
	}

	if (send(sockfd, header, strlen(header), 0) != strlen(header))
	{
		LOG_PERROR_INFO("send error.");
		goto _return_error_;
	}

	socketstate_t state;
	int ret = read_header(sockfd, header, MAXBUF, NULL, &state);
	if (ret <= 0)
	{
		LOG_ERROR_INFO("read header error!\n");
		goto _return_error_;
	}
	header[ret] = '\0';
	LOG_NORMAL_INFO("%s", header);

	//判断是否是chunked或者content-length
	int len = 0;
	int contentLen = get_content_length(header, ret);
	memset(buf, 0, MAXBUF);
	if (contentLen < 0)
	{
		//按照动态来接收
		LOG_WARN_INFO("can't find content length, try chunked ... \n");
		len = readChunkedText(sockfd, buf, MAXBUF);
		if (len <= 0)
		{
			LOG_ERROR_INFO("readChunkedText failt!\n");
			goto _return_error_;
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
			goto _return_error_;
		}
	}
	else
	{
		LOG_ERROR_INFO("content len == 0 !\n");
		return -1;
	}
	LOG_HL_INFO("%s\n", buf);

	char md5[36] = {0};
	int offset = strline(buf, md5, sizeof(md5));
	if (offset <= 0)
		goto _return_error_;
	if (isStrexit(md5, "error"))
		goto _return_error_;
	char *p = strchr(md5, '\r');
	if (p)
		*p = 0;
	p = strchr(md5, '\n');
	if (p)
		*p = 0;	
	char channelName[MAXBUF+100] = {0};
	offset = strline(buf + offset, channelName, MAXBUF);
	if (offset <= 0)
		goto _return_error_;
	p = strchr(channelName, '\r');
	if (p)
		*p = 0;
	p = strchr(channelName, '\n');
	if (p)
		*p = 0;
	memset(md5str, 0, sizeof(md5str));
	CalMd5(channelName, strlen(channelName), md5str);
	if (strcmp(md5, md5str))
	{
		LOG_ERROR_INFO("the md5sum exception, server md5: %s, local md5: %s, channelName: %s\n", md5, md5str, channelName);
		goto _return_error_;
	}
	if (offset > sizeof(g_channelName) - 1)
	{
		LOG_ERROR_INFO("channelName is too long, %d!", offset);
		goto _return_error_;
	}
	memset(g_channelName, 0, sizeof(g_channelName));
	memcpy(g_channelName, channelName, offset);
	g_channelNameLen = strlen(channelName);
	LOG_NORMAL_INFO("channelName = %s\n", g_channelName);

	close(sockfd);
	return 0;
	
_return_error_:
	close(sockfd);
	return -1;
}

void init_channel()
{
	while (1)
	{
		if (RequestChannelName() >= 0)
			break;
		sleep(5);
	}
}

