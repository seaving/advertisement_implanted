#include "includes.h"


void uci_commit()
{	
	char cmd[32] = {0};
	EXECUTE_CMD(cmd, "uci commit");
}

void write_uci_commit_cmd_shell()
{	
	char cmd[32] = {0};
	WRITE_EXECUTE_CMD_TO_FILE(cmd, "uci commit");
}

void restart_network()
{
	return;
	
	//char cmd[32] = {0};
	//EXECUTE_CMD(cmd, "/etc/init.d/network restart");
}

