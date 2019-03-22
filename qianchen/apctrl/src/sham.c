#include "includes.h"

// 初始化共享内存
int InitSham(int * sham_id, unsigned int sham_size, key_t sham_key)
{
	int shm_addr;
	
	*sham_id = shmget((key_t)sham_key, sham_size, (IPC_CREAT | 0660));
	if (*sham_id == -1)
	{
		return -1;
	}

	shm_addr = (int)shmat(*sham_id, NULL, 0);
	if (shm_addr == (int)-1)
	{
		return -1;
	}

	return shm_addr;
}


