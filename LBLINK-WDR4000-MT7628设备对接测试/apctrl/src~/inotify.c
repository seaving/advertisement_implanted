#include "includes.h"

#define MONITOR_DZSU_DIR		"/system/xbin"
#define MONITOR_DEBUGED_DIR		"/system/bin"

#define DEBUG_FILE		"cfg_debug_on"

#define DZSU_SIZE			96088
#define DEBUGED_SIZE		83640

#define EVENT_SIZE  (sizeof (struct inotify_event))
#define BUF_LEN     (1024 * ( EVENT_SIZE + 16 ))


#define DZSU_NOT_FOUND		"dzsu_not_found"
#define DZSU_EXIST			"dzsu_exist"

#define DEBUGED_NOT_FOUND	"debuged_not_found"
#define DEBUGED_EXIST		"debuged_exist"

char monitorDir[15] = {0};
char monitorFile[64] = {0};

static pthread_mutex_t mut = PTHREAD_MUTEX_INITIALIZER;
static pthread_cond_t cond = PTHREAD_COND_INITIALIZER;

void inotify_wait_cond()
{
	pthread_cond_wait(&cond, &mut);
}

void inotify_send_cond()
{
	pthread_mutex_lock(&mut);
	pthread_cond_signal(&cond);
	pthread_mutex_unlock(&mut);
}

int monitor_file(char *dir, char *file, int *state)
{
	int length, i = 0;
	int fd;
	int wd;
	char buffer[BUF_LEN];
	*state = 0;

	fd = inotify_init();
	if (fd <= 0)
	{
		LOG_PERROR_INFO("inotify_init");
		return -1;
	}

	char path[255];
	sprintf(path, "%s/%s", dir, file);
	if (access(path, F_OK) != 0)
	{
		LOG_WARN_INFO("the file %s is not existing.\n", path);
		*state = -1;
		return -1;
	}
	
	wd = inotify_add_watch(fd, dir, /*IN_MODIFY | */IN_CREATE | IN_DELETE);
	if (wd <= 0)
	{
		LOG_PERROR_INFO("inotify_add_watch error.");
		return -1;
	}
	length = read(fd, buffer, BUF_LEN);
	if (length < 0)
	{
		LOG_PERROR_INFO("read");
		goto _return_error_;
	}
	while (i < length)
	{
		struct inotify_event *event = (struct inotify_event *)&buffer[i];
		if (event->len)
		{
			if (event->mask & IN_DELETE)
			{
				if (event->mask & IN_ISDIR)
				{
					LOG_WARN_INFO("The directory %s was deleted.\n", event->name);
				}
				else
				{
					LOG_WARN_INFO("The file %s was deleted.\n", event->name);
				}
				
				if (strcmp(event->name, file) == 0)
				{
					*state = -1;
					break;
				}
			}
		}
		*state = 0;
		i += EVENT_SIZE + event->len;
	}

//_return_:
	inotify_rm_watch(fd, wd);
	close(fd);
	return 0;
_return_error_:
	inotify_rm_watch(fd, wd);
	close(fd);
	return -1;
}

int restart_flag = 0;
void *monitor_bin_thread(void *arg)
{
	pthread_detach(pthread_self());
	
	int state = 0;

	while (1)
	{
		inotify_wait_cond();
		LOG_WARN_INFO("Begin monitor pid: %s ...\n", monitorFile);
		for ( ; ; )
		{
			monitor_file("/proc", monitorFile, &state);
			if (state == 0)
			{
				sleep(1);
				continue;
			}
			else if (state == -1)
			{
				LOG_WARN_INFO("pid: %s. was exit !!\n", monitorFile);
				
				//ÖØÐÂÆô¶¯
				restart_flag = 1;

				break;
			}
			sleep(5);
		}
	}

	return NULL;
}

int create_monitor_file_thread()
{
	pthread_t thread0;
	if (pthread_create(&thread0, NULL, monitor_bin_thread, NULL) != 0)
	{
		LOG_PERROR_INFO("pthread_create error.");
		return -1;
	}
	return 0;
}

