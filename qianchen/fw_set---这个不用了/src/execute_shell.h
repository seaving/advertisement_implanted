#ifndef __EXECUTE_SHELL_H__
#define __EXECUTE_SHELL_H__

#include "debug.h"

#define EXECUTE_CMD(cmd_buf, first...) { \
	sprintf(cmd_buf, first); \
	LOG_HL_INFO("%s\n", cmd_buf); \
	execute(cmd_buf, 1); \
}

int execute(const char *cmd_line, int quiet);

#endif



