#include "includes.h"


sprocess_lock *process_spinlock_create(int size)
{
    //申请共享内存
    sprocess_lock *prolock;
    prolock = (sprocess_lock *)mmap(NULL, size, 
    	PROT_READ | PROT_WRITE, MAP_SHARED | MAP_ANONYMOUS, -1, 0);
    
    prolock->shm_size = size;
    prolock->pid_lock = 0;
    
    //初始化自旋转锁（spinlock=1）        
   // pthread_spin_init(&prolock->spinlock, 0);
    
    return prolock;
}

int process_getlock(sprocess_lock *prolock, int pid)
{
    int ret = 0;

    //自旋锁 加锁【为了保证下面操作原子性】
    //pthread_spin_lock(&prolock->spinlock);

	//未上锁
    if (prolock->pid_lock == 0)
    {
        prolock->pid_lock = pid;
       //printf("lock: pid_lock:%d,pid:%d\n", shttp_lock->pid_lock, pid);
    }
    else
    {
        ret = -1;
    }
    
    //自旋锁 解锁
   // pthread_spin_unlock(&prolock->spinlock);
    
    return ret;
}

int process_unlock(sprocess_lock *prolock)
{
    prolock->pid_lock = 0;
    return 0;
}


