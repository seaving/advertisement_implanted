#ifndef __DUP_FILE_H__
#define __DUP_FILE_H__	1

#define LOG_SAVE_ON	0

#if LOG_SAVE_ON
#define LOG_SAVE(first...) { \
	printf(first); \
	fflush(stdout); \
}
#else
#define LOG_SAVE(first...) ;
#endif

int dup_file();
int dup_close();

#endif

