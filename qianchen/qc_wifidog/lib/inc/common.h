
#ifndef  __COMMON_H__
#define  __COMMON_H__

#include <arpa/inet.h>
#include <errno.h>
#include <libgen.h>
#include <netdb.h>
#include <resolv.h>
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/ipc.h>

#include <sys/socket.h>
#include <unistd.h>
#include <sys/wait.h>
#include <sys/file.h>

#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <signal.h>

#include <sys/param.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <netdb.h>
#include <netinet/tcp.h>
#include <net/if.h>
#include <pthread.h>

#include <linux/sockios.h>
#include <sys/ioctl.h> 
#include <sys/time.h>
#include <sys/wait.h>
#include <netinet/if_ether.h> 
#include <sys/ipc.h>
#include <stdbool.h>
#include <stdarg.h>
#include <dirent.h>
#include <sys/mman.h>

#include <alloca.h>
#include <math.h>
#include <ctype.h>

#include <sys/system_properties.h>

#include <android/log.h>

#define LOG_TAG "DEBUG"
#define LOGD(fmt, args...) __android_log_print(ANDROID_LOG_DEBUG, LOG_TAG, fmt, ##args)

#define _exit_(t) {\
	return t; \
}
#define _continue_(t) {\
	sleep(t);\
	continue; \
}

#define EXECUTE_CMD(cmd_buf, first...) { \
	sprintf(cmd_buf, first); \
	printf("%s\n", cmd_buf); \
	system(cmd_buf); \
}

#define VAL_LOG(first...) {\
    printf("[%s]:%d ", __FUNCTION__, __LINE__);\
    printf(first);\
    fflush(stdout); \
}

#define VAL_PERROR(first...) {\
    printf("[%s]:%d ", __FUNCTION__, __LINE__);\
    printf(first);\
    perror(" ");\
    printf("\n");\
    fflush(stdout); \
}

#endif

