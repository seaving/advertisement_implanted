#include "includes.h"

void signal_sigpipe()
{
	signal(SIGPIPE, SIG_IGN);
}

int main()
{
	cap_t cap;
	
	signal_sigpipe();

	time_init();

	init_netdev();

	memset(&cap, 0, sizeof(cap_t));
	snprintf(cap.dev_name, 10, "br-lan");
	cap_init(&cap);
	
	for ( ; ; )
	{
		sleep(1);
	}

	return 0;
}

