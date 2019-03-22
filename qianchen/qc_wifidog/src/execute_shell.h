#ifndef __EXECUTE_SHELL_H__
#define __EXECUTE_SHELL_H__

#define EXECUTE_CMD(cmd_buf, first...) { \
	sprintf(cmd_buf, first); \
	LOG_HL_INFO("%s\n", cmd_buf); \
	/*execute(cmd_buf, 1);*/ \
	system(cmd_buf); \
}

int execute(const char *cmd_line, int quiet);

#endif

