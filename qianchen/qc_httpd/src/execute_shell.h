#ifndef __EXECUTE_SHELL_H__
#define __EXECUTE_SHELL_H__

#include "debug.h"

#define EXECUTE_CMD(cmd_buf, first...) { \
	sprintf(cmd_buf, first); \
	LOG_HL_INFO("%s\n", cmd_buf); \
	execute(cmd_buf, 1); \
}

#define WRITE_EXECUTE_CMD_TO_FILE(cmd_buf, first...) { \
	sprintf(cmd_buf, first); \
	char write_exeute_cmd_to_file_cmd_ccccccccsss[513] = {0}; \
	snprintf(write_exeute_cmd_to_file_cmd_ccccccccsss, 512, "echo \"%s\" >> \"/tmp/cmdsh.sh\"", cmd_buf); \
	LOG_HL_INFO("%s\n", write_exeute_cmd_to_file_cmd_ccccccccsss); \
	execute(write_exeute_cmd_to_file_cmd_ccccccccsss, 1); \
}


int execute(const char *cmd_line, int quiet);

#endif



