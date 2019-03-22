#include "includes.h"

//-c 包数目(ping次数) -W 超时 -s 包大小 域名/IP
#define PING_SHELL "echo '#!/bin/sh' > /tmp/ping.sh; \
echo 'domain=$1' >> /tmp/ping.sh; \
echo 'ping_count=$2' >> /tmp/ping.sh; \
echo 'ping_package_size=$3' >> /tmp/ping.sh; \
echo 'ping_result_file=\"/tmp/ping.result\"' >> /tmp/ping.sh; \
echo 'killall ping' >> /tmp/ping.sh; \
echo 'rm -rf $ping_result_file' >> /tmp/ping.sh; \
echo 'ping -c $ping_count -W 2 -s $ping_package_size $domain > $ping_result_file' >> /tmp/ping.sh; \
echo \"ping_result_size=\\`ls -al \\$ping_result_file | awk '{printf \\$5}'\\`\" >> /tmp/ping.sh; \
echo 'if [ \"$ping_result_size\" -gt 5 ]; then' >> /tmp/ping.sh; \
echo 'echo \"____ping_finished____\" >> /tmp/ping.result' >> /tmp/ping.sh; \
echo 'fi' >> /tmp/ping.sh; \
echo 'exit' >> /tmp/ping.sh;"

#define CMD_PING	"chmod +x /tmp/ping.sh;/tmp/ping.sh %s %d %d &"

void ping(int c, int s, char *domain)
{
	char cmd[1025] = {0};
	EXECUTE_CMD(cmd, "%s", PING_SHELL);
	EXECUTE_CMD(cmd, CMD_PING, domain, c, s);
}

void stop_ping()
{
	char cmd[255] = {0};
	EXECUTE_CMD(cmd, "killall ping");
}

int ping_result_size()
{
	return get_file_size("/tmp/ping.result");
}

char* get_result()
{
	char *result = NULL;
	int size = ping_result_size();
	if (size <= 0)
		return NULL;

	result = calloc(1, size + 1);
	if (! result)
		return NULL;

	read_file("/tmp/ping.result", result, size);

	return result;
}

bool is_ping_finished(char *result)
{
	return strstr(result, "____ping_finished____") ? true : false;
}

