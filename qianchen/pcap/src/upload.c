#include "includes.h"


#define UPLOAD_HOST		"log.skyroam.com.cn"
#define UPLOAD_PORT		9110
#define UPLOAD_URI		"/index?Hw=%s&Ver=%s&Sn=%s&Ch=%d&Type=%d"

static char g_boundary_header[] = "------WebKitFormBoundaryapMKTQABBP6vWIo0\r\n";
static char g_boundary_tail[] = "\r\n------WebKitFormBoundaryapMKTQABBP6vWIo0--\r\n";

static char _simo_sn[64] = {0};
static char _osi_ver[64] = {0};
static char _device_type[64] = {0};

static char _upload_server_ip[32] = {0};

typedef void (*popen_call)(char *, void *);
int popen_cmd(char *cmd, popen_call call, void *arg)
{
	FILE *fp = NULL;

	VAL_LOG("%s\n", cmd);

	fp = popen(cmd, "r");
	if (NULL == fp) 
	{  
		VAL_PERROR("popen error!");
		return -1;
	}

	char buf[255];
	while (fgets(buf, 255, fp) != NULL)
	{
		char *p = strchr(buf, '\n');
		if (p)
			*p = 0;
		p = strchr(buf, '\r');
		if (p)
			*p = 0;
		
		VAL_LOG("%s\n", buf);

		if (call)
			call(buf, arg);

		memset(buf, 0, sizeof(buf));
	}
	
	pclose(fp);
	
	return 0;
}

static void _getprop_call_(char *buf, void *value)
{
	if (buf && value)
		memcpy(value, buf, strlen(buf));
}

static int _getprop(char *key, char *value)
{
	char cmd[255] = {0};
	sprintf(cmd, "getprop %s", key);
	popen_cmd(cmd, _getprop_call_, value);
	return 0;
}

static void _upload_init()
{
	memset(_upload_server_ip, 0, sizeof(_upload_server_ip));
	while (strlen(_upload_server_ip) <= 0)
	{
		get_addr_info(UPLOAD_HOST, _upload_server_ip);
		sleep(5);
	}

	_getprop("simo_sn", _simo_sn);
	_getprop("osi_ver", _osi_ver);
	_getprop("device_name", _device_type);
}

char *get_sime_sn()
{
	return _simo_sn;
}

int get_file_size(const char *path)
{  
    int filesize = -1;
    FILE *fp;
    fp = fopen(path, "r");
    if(fp == NULL)
        return filesize;
    fseek(fp, 0L, SEEK_END);
    filesize = ftell(fp);
    fclose(fp);
    return filesize;
}

time_t get_file_create_time(char *file)
{
	struct stat buf;
	if (lstat(file, &buf) < 0)
	{
		VAL_PERROR("lstat error.");
		return -1;
	}
	
	return buf.st_ctime;
}

int save_data(char *data, int len)
{
	char file_name[128];
	static int index = 0;
	sprintf(file_name, "/data/local/tmp/gztraffic_%d.gz", index ++);
	int fd = open(file_name, O_CREAT | O_RDWR, S_IRUSR | S_IWUSR | S_IRGRP | S_IWGRP | S_IROTH);
	if (fd < 0)
		return -1;
	write(fd, data, len);
	close(fd);
	return 0;
}

static int _upload_traffic(char *file, char *filename)
{
	if (! file)
		return -1;
	
	char *data = NULL;
	int upfd = -1;

	int fd = open(file, O_RDWR);
	if (fd < 0)
	{
		VAL_PERROR("open error.");
		return -1;
	}

	int ret = -1;

	int file_size = get_file_size(file);
	if (file_size <= 0)
	{
		close(fd);
		return 0;
	}
	//VAL_LOG("file size: %d\n", file_size);
	
	//压缩数据
	int file_size_tmp = file_size;
	int read_len = 0;
	int offset = 0;
	char *gz_data = calloc(1, file_size + 4048);
	if (! gz_data)
	{
		goto err;
	}
	data = calloc(1, file_size + 4048);
	if (! data)
	{
		goto err;
	}
	sprintf(data, "ID: %s\n", filename);
	
	//VAL_LOG("data %s\n", data);
	upfd = open_clientfd_noblock(NULL, UPLOAD_HOST, UPLOAD_PORT);
	if (upfd < 0)
		goto err;
	
	socket_setfd_noblock(upfd);
	offset = strlen(data);
	while (file_size_tmp > 0)
	{
		if (file_size_tmp > 1024)
			read_len = 1024;
		else
			read_len = file_size_tmp;
		ret = read(fd, data + offset, read_len);
		if (ret <= 0)
		{
			VAL_PERROR("read error.");
			goto err;
		}

		offset += ret;
		data[offset] = 0;

		file_size_tmp -= ret;
	}
	VAL_LOG("=====================\n");
	VAL_LOG("len:%d\n%s\n", strlen(data), data);
	VAL_LOG("=====================\n");

	int gz_data_len = 0;
	file_size_tmp = file_size + 4048;
	gzcompress((Bytef *)data, (uLong)offset, (Bytef *)gz_data, (uLong *)&file_size_tmp);
	if (file_size_tmp <= 0)
	{
		VAL_LOG("gzcompress failt !\n");
		goto err;
	}
	gz_data_len = file_size_tmp;

	file_size_tmp += (strlen(g_boundary_header) + strlen(g_boundary_tail));

	int data_type = 4;
	if (strstr(filename, "_dns-"))
		data_type = 5;
	else
		data_type = 4;

	_getprop("simo_sn", _simo_sn);
	_getprop("osi_ver", _osi_ver);
	_getprop("device_name", _device_type);

	sprintf(data, "POST " UPLOAD_URI " HTTP/1.1\r\nHost: %s:%d\r\nContent-length: %d\r\n\r\n", 
		_device_type, _osi_ver, _simo_sn, 1, data_type, UPLOAD_HOST, UPLOAD_PORT, file_size_tmp);
	offset = strlen(data);

	memcpy(data+offset, g_boundary_header, strlen(g_boundary_header));
	offset += strlen(g_boundary_header);
	memcpy(data+offset, gz_data, gz_data_len);
	offset += gz_data_len;
	memcpy(data+offset, g_boundary_tail, strlen(g_boundary_tail));
	offset += strlen(g_boundary_tail);
	VAL_LOG("offset=%d\n", offset);
	if (socket_send(upfd, data, offset, 10) != offset)
		goto err;

	VAL_LOG(">>>>> %s", data);

	//接收响应
	ret = socket_read(upfd, data, 1024, 20);
	if (ret <= 0)
	{
		nl_log("eeeeeeeee socket read error!\n");
		goto err;
	}
	data[ret] = '\0';
	nl_log("@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@\n");
	nl_log("%s", data);
	nl_log("@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@\n");

	//判断是否为200
	char *p = strstr(data, "\r\n");
	if (p)
	{
		*p = 0;
		if (! strstr(data, " 200"))
			goto err;
	}
	else
	{
		goto err;
	}
	
//done:
	if (gz_data) free(gz_data);
	if (data) free(data);
	close(fd);
	socket_close(upfd);
	return 0;
	
err:
	if (gz_data) free(gz_data);
	if (data) free(data);
	close(fd);
	socket_close(upfd);
	return -1;
}

void upload_last()
{
	struct dirent* ent = NULL;
	DIR *pDir = NULL;
	struct stat statbuf;
	pDir = opendir(TRAFFIC_DIR);
	if (pDir == NULL)
	{
		VAL_PERROR("opendir error.");
		return ;
	}

    while (NULL != (ent = readdir(pDir)))
    {
    	char path[255] = {0};
    	sprintf(path, TRAFFIC_DIR"/%s", ent->d_name);
    	if (strcmp(ent->d_name, ".") == 0
    	|| strcmp(ent->d_name, "..") == 0)
    		continue;
    	
    	lstat(path, &statbuf);
        if (! S_ISDIR(statbuf.st_mode))
        {
        	//VAL_LOG("last file: %s\n", path);

			//更名成上传文件
			if (ent->d_name[0] == '.')
    		{
				//把临时保存的文件更名，这样就上传线程便可以识别进行上传
				char file_tmp[128];
				sprintf(file_tmp, "%s", path);
				char *p = strchr(file_tmp, '.');
				if (p)
					*p = '0';
				if (rename(path, file_tmp) != 0)
					unlink(path);
				VAL_LOG("%s --> %s\n", path, file_tmp);
    		}
        }
    }

	closedir(pDir);
}

static bool _check_network(char *host, int port)
{
	int socket = open_clientfd_noblock(NULL, host, port);
	if (socket > 0)
	{
		nl_log("network check ok .\n");
		close(socket);
		return true;
	}

	nl_log("network check error .\n");

	return false;
}

static int _upload_file(char *path, char *file_name)
{
	/**
		每隔30秒重传传一次文件,有网情况下重传3次失败，则删除文件
		其他情况重试三次直接返回不删除文件
	*/
	int i = 0, j = 0;
	for (i = 0; i < RE_UPLOAD_MAX_COUNT + 1; i ++)
	{
		nl_log("[%d] upload file: %s\n", i, path);

		//判断是否有网
		if (_check_network("www.baidu.com", 80) == true)
		{
			j ++;
			
			//上传接口
			if (_upload_traffic(path, file_name) == 0)
			{
				nl_log("[%d] _upload_traffic succes: unlink %s\n", i, path);
				unlink(path);

				return 0;
			}
			nl_log("[%d] _upload_traffic failt: %s\n", i, path);
		}
		
		sleep(30);
	}

	if (j >= RE_UPLOAD_MAX_COUNT)
	{
		nl_log("upload count reach: %d, unlink %s\n", j, path);
		unlink(path);
	
		return 0;
	}

	return -1;
}

//扫描线程
bool run_begin = false;
static int _upload()
{
	_upload_init();

	//程序启动第一时间检测上次遗留下来的临时报表
	upload_last();

	run_begin = true;

	int upload_file_num = 0;

	for ( ; ; )
	{
		struct dirent* ent = NULL;
		DIR *pDir = NULL;
		struct stat statbuf;
		pDir = opendir(TRAFFIC_DIR);
		if (pDir == NULL)
		{
			VAL_PERROR("opendir error.");
			return -1;
		}

		//扫描文件夹后一次性上传所有的文件
	    while (NULL != (ent = readdir(pDir)) 
	    && upload_file_num < UPLOAD_FILE_MAX_COUNT)
	    {
	    	char path[255] = {0};
	    	sprintf(path, TRAFFIC_DIR"/%s", ent->d_name);
	    	
	    	if (ent->d_name[0] == '.')
	    		continue;
	    	lstat(path, &statbuf);
	        if (! S_ISDIR(statbuf.st_mode))
	        {
	        	_upload_file(path, ent->d_name);

				//统计上传文件个数
	        	upload_file_num ++;
	        }

	        nl_log("upload_file_num: %d\n", upload_file_num);
	    }

		closedir(pDir);

		//清空，一小时后开始重新下一轮上传统计
		upload_file_num = 0;

		//休眠一个小时后继续扫描文件夹
		sleep(60 * 55);
	    //usleep(200 * 1000);
	}
	
	return 0;
}

char *get_upload_server_ip()
{
	return _upload_server_ip;
}

void *upload_thread(void *arg)
{
	pthread_detach(pthread_self());
	_upload();
	return NULL;
}

int create_upload_thread()
{
	pthread_t thd;
	return pthread_create(&thd, NULL, upload_thread, NULL);
}

