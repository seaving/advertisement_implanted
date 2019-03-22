#include "includes.h"


void ppool_ignore(int signal)	
{	
	;	
} 

//进程自动回收
void signal_task_exit()
{
	signal(SIGCHLD, SIG_IGN);
}

void signal_sigpipe()
{
	signal(SIGPIPE, SIG_IGN);
}
