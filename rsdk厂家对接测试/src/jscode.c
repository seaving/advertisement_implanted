#include "includes.h"

#define JS_SERVER_HOST		"121.196.215.235"
#define JS_HTTP_URI			"/plugin/api/loadJS.jhtml"
#define JS_SERVER_PORT		80
#define JS_SAVE_FILE_TMP	"/tmp/js.txt"

char *hadoop_js_code_h = NULL;
char *hadoop_js_code_e = NULL;
int hadoop_js_code_h_len = 0;
int hadoop_js_code_e_len = 0;

char *get_js_code_h()
{
	return hadoop_js_code_h;
}
char *get_js_code_e()
{
	return hadoop_js_code_e;
}
int get_js_code_h_len()
{
	return hadoop_js_code_h_len;
}
int get_js_code_e_len()
{
	return hadoop_js_code_e_len;
}

int get_js_code_from_server()
{

	int serverfd = -1;
	char *js_file = JS_SAVE_FILE_TMP;
	int js_fd = -1;

	char header_buff[512];
	char path[256];
	char parm[256];
	char *parm_tmp = parm;
	char *jsbuf = NULL;

	int js_len = 0;
	serverfd = connecthost(JS_SERVER_HOST, JS_SERVER_PORT, E_CONNECT_BLOCK);
	if (serverfd <= 0)
	{
		LOG_ERROR_INFO("connect error to %s:%d!\n", JS_SERVER_HOST, JS_SERVER_PORT);
		goto _return_error_;
	}

	int offset = 0;
	offset = js_add_param("?action=", "update", parm_tmp);
	parm_tmp += offset;
	offset = js_add_param("&channelId=", ID, parm_tmp);
	parm_tmp += offset;
	offset = js_add_param("&version=", VERSION, parm_tmp);
	parm_tmp += offset;
	offset = js_add_param("&routerMac=", get_netdev_wan_mac(), parm_tmp);
	parm_tmp += offset;
	
 	unsigned char md5sum[16] = {0};
 	memcpy(parm_tmp, TOKEN_KEY, TOKEN_KEY_LEN);
	myMd5((unsigned char *)parm, parm_tmp - parm + TOKEN_KEY_LEN, (unsigned char *)md5sum);
	char md5str[33] = {0};
	sprintf(md5str, "%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x", 
			md5sum[0], md5sum[1], md5sum[2], md5sum[3], md5sum[4], md5sum[5], md5sum[6],
			md5sum[7], md5sum[8], md5sum[9], md5sum[10], md5sum[11], md5sum[12], md5sum[13],
			md5sum[14], md5sum[15]);
	offset = js_add_param("&md5=", md5str, parm_tmp);
	parm_tmp += offset;
	*parm_tmp = '\0';

	sprintf(path, "%s%s", JS_HTTP_URI, parm);
	
	struct_http_header("GET", path, JS_SERVER_HOST, header_buff);
	LOG_NORMAL_INFO("%s", header_buff);
	
	int header_len = strlen(header_buff);
	if (socket_send(serverfd, header_buff, header_len) != header_len)
	{
		LOG_ERROR_INFO("send js code server error!\n");
		goto _return_error_;
	}

	char buf[MAXBUF + 1];
	socketstate_t state = E_ERROR;
	int ret = read_header(serverfd, buf, MAXBUF, NULL, &state);
	if (ret <= 0)
	{
		LOG_ERROR_INFO("read js header error!\n");
		goto _return_error_;
	}
	buf[ret] = 0;
	LOG_NORMAL_INFO("%s", buf);

	//获取长度
	int contextlen = get_content_length(buf, ret);
	if (contextlen <= 0)
	{
		LOG_ERROR_INFO("js code context length <= 0!\n");
		goto _return_error_;
	}

	js_fd = open(js_file, O_RDWR | O_CREAT | O_TRUNC);
	if (js_fd <= 0)
	{
		LOG_PERROR_INFO("open %s error!", js_file);
		goto _return_error_;
	}

	int readlen = 0;
	while (contextlen > 0)
	{
		if (contextlen > MAXBUF)
		{
			readlen = MAXBUF;
		}
		else
		{
			readlen = contextlen;
		}
		ret = socket_read(serverfd, buf, readlen, &state);
		if (ret <= 0) {
			break;
		}
		buf[ret] = '\0';
		contextlen -= ret;
		js_len += ret;
		if (write(js_fd, buf, ret) <= 0) {
			LOG_PERROR_INFO("write error for js request!");
			goto _return_error_;
		}
	}
	if (js_len <= 0)
	{
		LOG_ERROR_INFO("can not get js code from server!\n");
		goto _return_error_;
	}

	lseek(js_fd, 32+2, SEEK_SET);
	js_len -= 34;
	if (js_len <= 0)
	{
		LOG_ERROR_INFO("can not get js code from server!\n");
		goto _return_error_;
	}
	
	jsbuf = malloc(js_len);
	if (jsbuf == NULL)
	{
		LOG_PERROR_INFO("malloc error!\n");
		goto _return_error_;
	}
	ret = read(js_fd, jsbuf, js_len);
	if (ret < js_len)
	{
		LOG_PERROR_INFO("read error, ret = %d, js_len = %d!\n", ret, js_len);
		goto _return_error_;
	}
	*(jsbuf + ret) = '\0';
	js_len = ret;

	LOG_NORMAL_INFO("%s\n", jsbuf);

#if 1
	offset = find_str(jsbuf, js_len, "#code#", strlen("#code#"));
	if (offset < 0)
	{
		LOG_ERROR_INFO("can not find '#code#' !\n");
		goto _return_error_;
	}

	hadoop_js_code_h_len = offset;
	hadoop_js_code_e_len = js_len - offset - strlen("#code#");
	
	hadoop_js_code_h = malloc(hadoop_js_code_h_len + 1);
	if (hadoop_js_code_h == NULL)
	{
		LOG_PERROR_INFO("malloc error!");
		free_malloc(jsbuf);
		goto _return_error_;
	}
	hadoop_js_code_e = malloc(hadoop_js_code_e_len + 1);
	if (hadoop_js_code_e == NULL)
	{
		LOG_PERROR_INFO("malloc error!");
		free_malloc(jsbuf);
		free_malloc(hadoop_js_code_h);
		goto _return_error_;
	}

	memcpy(hadoop_js_code_h, jsbuf, hadoop_js_code_h_len);
	hadoop_js_code_h[hadoop_js_code_h_len] = '\0';
	char *paddr = jsbuf + offset + strlen("#code#");
	memcpy(hadoop_js_code_e, paddr, hadoop_js_code_e_len);
	hadoop_js_code_e[hadoop_js_code_e_len] = '\0';

	free_malloc(jsbuf);

	close(js_fd);
	unlink(js_file);
	close(serverfd);
	return 0;
#endif	
_return_error_:
	free_malloc(jsbuf);
	close(js_fd);
	unlink(js_file);
	close(serverfd);
	return -1;
}

void free_js()
{
	if (hadoop_js_code_h)
	{
		free(hadoop_js_code_h);
		hadoop_js_code_h = NULL;
		hadoop_js_code_h_len = 0;
	}
	if (hadoop_js_code_e)
	{
		free(hadoop_js_code_e);
		hadoop_js_code_e = NULL;
		hadoop_js_code_e_len = 0;
	}	
}

int js_add_param(char *param_name, char *param_arg, char *addr)
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

int js_add_param_n(char *param_name, int param_name_len, char *param_arg, int param_arg_len, char *addr)
{
	int len, bufsize;
	char *pjs = addr;
	len = param_name_len;
	memcpy(pjs, param_name, len);
	pjs += len;
	len = param_arg_len;
	bufsize = len * 4 + 1;
	char code[bufsize];
	len = url_encode((unsigned char *)param_arg, len, (unsigned char *)code, bufsize);
	memcpy(pjs, code, len);
	pjs += len;
	return pjs - addr;
}

int struct_js_parma(int sockfd, char *js, char *url, int url_len, char *title, int title_len, char *keyword, int keyword_len)
{
	if (js == NULL || url == NULL || strlen(url) <= 0) {
		return -1;
	}

	char phone_mac[20] = {0};
	if (get_peer_mac(sockfd, phone_mac) < 0) {
		sprintf(phone_mac, "00:00:00:00:00:00");
	}
	
	/**hadoop*/
	char *local_mac = get_netdev_wan_mac();
	
	int offset = 0;
	char *pjs = js;
	*pjs ++ = '?';
	memcpy(pjs, URL_TOKEN, URL_TOKEN_LEN);
	pjs += URL_TOKEN_LEN;

	offset = js_add_param_n("&channelId=", 11, ID, strlen(ID), pjs);
	pjs += offset;	
	//offset = js_add_param_n("&advertId=", 10, "1", 1, pjs);
	//pjs += offset;
	offset = js_add_param_n("&softwareVersion=", 17, VERSION, strlen(VERSION), pjs);
	pjs += offset;
	offset = js_add_param_n("&routerMac=", 11, local_mac, strlen(local_mac), pjs);
	pjs += offset;
	offset = js_add_param_n("&mac=", 5, phone_mac, strlen(phone_mac), pjs);
	pjs += offset;
	offset = js_add_param_n("&url=", 5, url, url_len, pjs);
	pjs += offset;
	
 	unsigned char md5sum[16] = {0};
 	memcpy(pjs, TOKEN_KEY, TOKEN_KEY_LEN);
	myMd5((unsigned char *)js, pjs - js + TOKEN_KEY_LEN, (unsigned char *)md5sum);
	char md5str[33] = {0};
	sprintf(md5str, "%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x", 
			md5sum[0], md5sum[1], md5sum[2], md5sum[3], md5sum[4], md5sum[5], md5sum[6],
			md5sum[7], md5sum[8], md5sum[9], md5sum[10], md5sum[11], md5sum[12], md5sum[13],
			md5sum[14], md5sum[15]);
	offset = js_add_param("&md5=", md5str, pjs);
	pjs += offset;

	*pjs = 0;
	/**end*/

	return pjs - js;
}

