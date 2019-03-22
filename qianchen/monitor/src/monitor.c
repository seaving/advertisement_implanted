#include "includes.h"

#define MPROXY_PID_PARENT_DIR	"/tmp/qcrun/mproxy_parent"
#define MPROXY_PID_SUB_DIR	"/tmp/qcrun/mproxy_sub"

void restart_program()
{
	char cmd[255];
	EXECUTE_CMD(cmd, "rm -rf "MPROXY_PID_PARENT_DIR"/*");
	EXECUTE_CMD(cmd, "rm -rf "MPROXY_PID_SUB_DIR"/*");

	EXECUTE_CMD(cmd, "killall mproxy_arm");

	msg_send(NULL, 0, FRAME_MODULE_MONITOR, FRAME_MODULE_FW_SET, FRAME_CMD_MITM_DELET);
	sleep(15);
	
	EXECUTE_CMD(cmd, "/tmp/app/mproxy_arm &");
}

int get_mproxy_pid(char *pidpath)
{
	int pid = -9999;
	
	struct dirent* ent = NULL;
	DIR *pDir = NULL;
	struct stat statbuf;
	
	pDir = opendir(pidpath);
	if (pDir == NULL)
	{
		LOG_PERROR_INFO("opendir %s error.", pidpath);
		return -1;
	}

	while (NULL != (ent = readdir(pDir)))
	{
		LOG_WARN_INFO("ent->d_name: %s\n", ent->d_name);
		char path[320] = {0};
		sprintf(path, "%s/%s", pidpath, ent->d_name);
		lstat(path, &statbuf);
		if (! S_ISDIR(statbuf.st_mode))
		{
			LOG_WARN_INFO("file: %s\n", path);
			pid = atoi(ent->d_name);
			break;
		}
	}
	
	closedir(pDir);
	
	return pid;
}


int read_file(char *file, char *buf, int len)
{
	int fd = open(file, O_RDWR);
	if (fd < 0)
	{
		LOG_PERROR_INFO("open %s error.", file);
		return -1;
	}
	
	int ret = read(fd, buf, len);
	if (ret <= 0)
	{
		LOG_PERROR_INFO("read error.");
	}

	close(fd);

	return ret;
}

static int _monitor_mproxy()
{
	int i = 0, timecnt = 0, ret = -1;
	char path[64] = {0};
	char comm[64] = {0};
	int pid[2] = {-9999, -9999};
	pid[0] = get_mproxy_pid(MPROXY_PID_PARENT_DIR);
	LOG_HL_INFO("parent pid: %d\n", pid[0]);
	if (pid[0] <= 0)
	{
		return -1;
	}

	//300秒 5分钟 等待子进程启动
	for (timecnt = 0; timecnt < 30; timecnt ++)
	{
		LOG_HL_INFO("wait mproxy_arm sub process start ...\n");
		pid[1] = get_mproxy_pid(MPROXY_PID_SUB_DIR);
		LOG_HL_INFO("sub pid: %d\n", pid[1]);
		if (pid[1] > 0)
		{
			break;
		}

		sleep(10);
	}

	pid[1] = get_mproxy_pid(MPROXY_PID_SUB_DIR);
	if (pid[1] <= 0)
	{
		return -1;
	}

	LOG_WARN_INFO("Begin monitor ... p: %d, s: %d.\n", pid[0], pid[1]);

	for ( ; ; )
	{
		for (i = 0; i < 2; i ++)
		{
			snprintf(path, sizeof(path), "/proc/%d/comm", pid[i]);
			LOG_HL_INFO("check pid: %s\n", path);
			memset(comm, 0, sizeof(comm));
			ret = read_file(path, comm, sizeof(comm) - 1);
			if (ret <= 0)
			{
				LOG_ERROR_INFO("can't read %s . error.\n", path);
				return -1;
			}
			
			if (! strstr(comm, "mproxy_arm"))
			{
				LOG_ERROR_INFO("the pid is not mroxy_arm! it was %s.\n", comm);
				return -1;		
			}
			
			sleep(3);
		}

		sleep(10);
	}
	
	return 0;
}

void *monitor_mproxy(void *arg)
{
	pthread_detach(pthread_self());
	
	while (1)
	{
		_monitor_mproxy();
		
		//重新启动
		restart_program();

		sleep(90);
	}
	
	return NULL;
}

int create_monitor_mproxy_thread()
{
	pthread_t thd;
	if (pthread_create(&thd, NULL, monitor_mproxy, NULL) != 0)
	{
		LOG_PERROR_INFO("pthread_create error.");
		return -1;
	}

	return 0;
}

