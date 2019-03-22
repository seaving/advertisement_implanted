#include "includes.h"

int g_ProcessArr[PROCESS_NUM] = {0};

int process_CreateSub()
{
    int i;
    for (i = 0; i < PROCESS_NUM; i ++)
    {
        int pid = fork();
        if (pid == 0)
        {
            return 1;
        }
        else if (pid > 0)
        {
            g_ProcessArr[i] = pid;
        }
        else
        {
            perror("create subProcess error");
            return -1;
        }
    }
    return 0;
}

int monitor_subprocess()
{
	int i = 0;
	
	int now_time_sec = 0;
	static int old_time_sec = -1;
	static int diff_cnt = -1;
	GetAllSystemTime(NULL, NULL, NULL, NULL, NULL, &now_time_sec, NULL);
	if (now_time_sec != old_time_sec)
	{
		diff_cnt ++;
		old_time_sec = now_time_sec;
	}
	else
	{
		return 0;
	}

	if (diff_cnt > 2)
	{
		diff_cnt = 0;
		for (i = 0; i < PROCESS_NUM; i ++)
		{
			char pid_path[64];
			sprintf(pid_path, "/proc/%d", g_ProcessArr[i]);
			DIR * pDirHandle = NULL;
			pDirHandle = opendir(pid_path);
			if (NULL == pDirHandle)
			{
				//该进程退出,我这里不选择重新生成进程
				//直接退出，由脚本重新运行
				printf("process pid: %d is not runing!", g_ProcessArr[i]);
				_exit_(-1);
			}
			else
			{
				closedir(pDirHandle);
			}
		}
	}
	
	return 0;
}


