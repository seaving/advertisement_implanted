#include "includes.h"

#define UPDATE_HOST_1	"http://" SERVER_HOST

#define SHELL_FILE		"/tmp/app/start.sh"
#define UPDATE_PATH		"/tmp/app.tar"
#define DOWNLOAD_PATH	"/tmp/app_dl.tar"
#define VERSION_TEXT	"/tmp/app/version.txt"

int extract_plugin(char *file, int size);

int check_result(char *res)
{
	LOG_NORMAL_INFO("%s\n", res);
	if (strcmp(res, "10000") == 0)
	{
		//更新
		LOG_HL_INFO("need update!\n");
		return 1;
	}
	if (strcmp(res, "10011") == 0)
	{
		LOG_ERROR_INFO("server not found plugin!\n");
		return 0;
	}
	if (strcmp(res, "10012") == 0)
	{
		LOG_ERROR_INFO("current plugin was lastest!\n");
		return 0;
	}

	return -1;
}

//1 下载成功，0 无须更新， -1 更新失败
int download(char *url)
{
	char *buf = calloc(1, MAXBUF + 1);
	if (! buf)
		return -1;

	int result = -1;
	char host[256] = {0};
	char path[512] = {0};
	int port = -1;
	int buf_len = -1;
	int ret = -1;
	int downfd = -1;
	int contentLen = 0;

	LOG_HL_INFO("url: %s\n", url);
	
	get_url_path(url, host, path, &port);
	if (strlen(host) <= 0 || port <= 0)
	{
		goto done;
	}

	downfd = open_clientfd_noblock(NULL, host, port);
	if (downfd < 0)
	{
		goto done;
	}

	sprintf(buf, "GET %s HTTP/1.1\r\n"
				"Host: %s:%d\r\n"
				"User-Agent: MSIE\r\n"
				"\r\n", path, host, port);
	LOG_NORMAL_INFO("%s", buf);

	buf_len = strlen(buf);
	ret = socket_send(downfd, buf, buf_len);
	if (ret != buf_len)
	{
		LOG_PERROR_INFO("send download header error.");
		goto done;
	}

	ret = read_header(downfd, buf, MAXBUF-1, NULL);
	if (ret <= 0)
	{
		LOG_ERROR_INFO("read download respons error!\n");
		goto done;
	}
	buf[ret] = 0;
	buf_len = ret;

	LOG_NORMAL_INFO("download response: %s\n", buf);

	contentLen = get_content_length(buf, buf_len);
	if (contentLen < 0)
	{
		LOG_WARN_INFO("can't find content length, try chunked ... \n");
		contentLen = readChunkedTextToFile(downfd, DOWNLOAD_PATH);
		if (contentLen <= 0)
		{
			goto done;
		}

	}
	else if (contentLen > 0)
	{
		contentLen = readStaticTextToFile(downfd, contentLen, DOWNLOAD_PATH);
		if (contentLen <= 0)
		{
			goto done;
		}
	}
	else
	{
		goto done;
	}

	result = 1;

done:
	if (downfd > 0) close(downfd);
	free_malloc(buf);
	return result;
}

int extract_upate_file()
{
	remove(FILE_PATH);

	mkdir(FILE_PATH, 0777);

	if (extract_plugin(DOWNLOAD_PATH, get_file_size(DOWNLOAD_PATH)) < 0)
	{
		return -1;
	}

	//解压
	char tar_cmd[255];
	EXECUTE_CMD(tar_cmd, "tar -xvf %s -C %s", UPDATE_PATH, FILE_PATH);
	
	remove(UPDATE_PATH);
	
	//执行包中的脚本
	EXECUTE_CMD(tar_cmd, "chmod +x %s", SHELL_FILE);
	EXECUTE_CMD(tar_cmd, "%s", SHELL_FILE);

	sleep(10);

	return 1;
}

int get_plugin_url(char *url)
{
	int ret = -1;
	
	char devNo[33] = {0};
	get_dev_id(devNo);

	char gccv[65] = {0};
	get_gcc_version(gccv);
	
	char pv[65] = {0};
	get_plugin_version(pv);
	if (strlen(pv) <= 0)
		snprintf(pv, 64, "v0.0.0");
	
	LOG_WARN_INFO(
		"\n"
		"devNo: %s\n"
		"gccv: %s\n"
		"pv: %s\n",
		devNo, gccv, pv
	);
	
	json_object *my_object = json_object_new_object();
	if (! my_object) return -1;
	
	json_object_object_add(my_object, "devNo", json_object_new_string(devNo));
	json_object_object_add(my_object, "gccv", json_object_new_string(gccv));
	json_object_object_add(my_object, "pv", json_object_new_string(pv));
	
	char *json_str = (char *) json_object_to_json_string(my_object);
	LOG_HL_INFO("JSON -->: %s\n", json_str);
	
	char response[513] = {0};
	http_send_post_data(SERVER_HOST, "/api/router/plugin", 80, json_str, "application/json", response, 512);
	LOG_NORMAL_INFO("---- %s\n", response);
	json_object_put(my_object);

	json_object *response_json = json_tokener_parse(response);
	if (is_error(response_json))
	{
		return -1;
	}

	const char *code = "";
	const char *data = "";
	const char *json_data = NULL;
	//const char *token = "";
	
	json_object *object_code;
	if (json_object_object_get_ex(response_json, "code", &object_code))
	{
		code = json_object_get_string(object_code);
	}
	
	json_object *object_data;
	if (json_object_object_get_ex(response_json, "data", &object_data))
	{
		data = json_object_get_string(object_data);
		json_data = json_object_to_json_string(object_data);
	}

	json_object *object_token;
	if (json_object_object_get_ex(response_json, "token", &object_token))
	{
		//token = json_object_get_string(object_token);
	}

	//LOG_NORMAL_INFO("code: %s, data: %s, token: %s\n", code, data, token);

	ret = check_result((char *) code);
	if (ret == 1)
	{
		char md5[33] = {0};
		CalMd5ByCopy((char *) (json_data ? json_data : ""), strlen(json_data), DATA_TOKEN_KEY, DATA_TOKEN_KEY_LEN, md5);
	
		if (/*strcmp(md5, token) == 0 &&*/ 0 < strlen(data) && strlen(data) <= 512)
		{
			ret = 1;
			memcpy(url, data, strlen(data));
		}
		else
		{
			//LOG_ERROR_INFO("cal_md5: %s, net_md5: %s, data_len: %d\n", md5, token, strlen(data));
		}	
	}

	json_object_put(response_json);

	return ret;
}

int get_plugin(int fd)
{
	char buf[1024];
	int wfd = -1, len = 0;
	unlink(UPDATE_PATH);
	create_file(UPDATE_PATH);
	wfd = open_file(UPDATE_PATH);
	if (wfd < 0)
		return -1;

	for ( ; ; )
	{
		len = read(fd, buf, 1024);
		if (len <= 0)
		{
			break;
		}

		write(wfd, buf, len);
	}

	close(wfd);
	return 0;
}

int analysis_plugin(char *file, int size, char *ver, char *gccv, char *md5)
{
	int result = -1;
	char buf[2048] = {0};

	int fd = open_file(file);
	if (fd < 0)
	{
		return -1;
	}
	
	int read_len = readline(fd, buf, 1024);
	if (read_len <= 0)
	{
		goto done;
	}

	char *ptmp = buf + strlen("###!!!;;;");
	char *p = strchr(ptmp, ';');
	if (! p)
		goto done;

	if (p - ptmp > 32)
		goto done;

	memcpy(ver, ptmp, p - ptmp);
	ptmp = p + 1;

	p = strchr(ptmp, ';');
	if (! p)
		goto done;

	if (p - ptmp > 64)
		goto done;

	memcpy(gccv, ptmp, p - ptmp);
	ptmp = p + 1;

	p = strchr(ptmp, ';');
	if (! p)
		goto done;

	if (p - ptmp > 32)
		goto done;

	memcpy(md5, ptmp, p - ptmp);
	ptmp = p + 1;

	result = get_plugin(fd);
	
done:
	close(fd);
	return result;
}

bool check_fwv(char *fwv)
{
	char myFwv[65] = {0};
	get_firmware_version(myFwv);
	return strcmp(myFwv, fwv) ? false : true;
}

bool check_gccv(char *gccv)
{
	char myGccv[65] = {0};
	get_gcc_version(myGccv);
	return strcmp(myGccv, gccv) ? false : true;
}

int extract_plugin(char *file, int size)
{
	char cal_md5[33] = {0};
	char plugin_md5[33] = {0};

	char ver[33] = {0};
	char gccv[65] = {0};
	if (analysis_plugin(file, size, ver, gccv, plugin_md5) < 0)
	{
		LOG_ERROR_INFO("analysis_plugin failt!\n");
		goto error;
	}
	
	if (! check_gccv(gccv))
		goto error;

	//校验文件
	memset(cal_md5, 0, sizeof(cal_md5));
	myMd5File(UPDATE_PATH, 0, get_file_size(UPDATE_PATH), NULL, 0, cal_md5);
	if (strcmp(cal_md5, plugin_md5))
	{
		LOG_ERROR_INFO("plugin md5 error! res: %s, cal: %s\n", plugin_md5, cal_md5);
		goto error;
	}
	
	unlink(DOWNLOAD_PATH);
	return 0;
	
error:
	unlink(UPDATE_PATH);
	return -1;
}

int RequestUpdate()
{
	char url[513]= {0};
	int ret = get_plugin_url(url);
	if (ret == 0)
	{
		//没有更新
		LOG_SAVE("no new high version program file to update.\n");
		return 0;
	}
	
	if (strlen(url) <= 0)
	{
		return -1;
	}

 	ret = download(url);
	if (ret < 0)
	{
		//更新出错
		LOG_SAVE("update error!\n");
		return -1;
	}
	else
	{
		return extract_upate_file();
	}

	sleep(3);
	
	return 0;
}

static bool _update_flag = false;
bool isResUpdata()
{
	return _update_flag;
}
void CleanResUpdata()
{
	_update_flag = false;
}
void SetResUpdata()
{
	_update_flag = true;
}

void *update_work(void *arg)
{
	pthread_detach(pthread_self());

	int cur_time = 0;
	int old_time = 0;
	bool update_ok = false;

	//检查是否有外网
	while (1)
	{
		char ip[32] = {0};
		if (get_dev_ip(ip, get_netdev_wan_name()) >= 0)
		{			
			LOG_NORMAL_INFO("the wan ip: %s\n", ip);
			break;
		}

		LOG_ERROR_INFO("get wan ip error!\n");

		sleep(5);
	}

	SetResUpdata();
	
	while (1)
	{
		cur_time = get_system_clock_sec();
		if (isResUpdata() || abs(cur_time - old_time) > 1 * _1_HOUR_)
		{
			//赋值当前时间,便于下次计数
			old_time = cur_time;

			//清除标记
			CleanResUpdata();

			//开始请求更新，返回0表示无须更新，1表示更新成功，<0表示更新出错
			int ret = RequestUpdate();
			if (ret > 0)
			{
				//更新成功，准备重新启动新的更新包
				LOG_SAVE("update success, restart program .\n");
				update_ok = true;
			}
			else if (ret == 0)
			{
				//无须更新,等1小时候再次更新
			}
			else
			{
				//更新失败,隔小段时间间隔再次去请求
			}			
		}

		if (update_ok)
		{
			update_ok = false;
			//不需要发送重启消息，因为start脚本做了一切
			//msg_send(NULL, 0, FRAME_MODULE_INIT, FRAME_MODULE_MONITOR, FRAME_CMD_RESTART_PROGRAM);
		}

		sleep(3);
	}
	
	return NULL;
}

int create_update_thread()
{
	pthread_t tdp;
	return pthread_create(&tdp, NULL, update_work, NULL);
}

