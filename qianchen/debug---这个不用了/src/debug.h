
#ifndef __DEBUG_H__
#define __DEBUG_H__
#include "common.h"
#include "id.h"

/* printf colour */
#define COLOR_NONE_C       "\033[m"
#define COLOR_RED          "\033[0;32;31m"
#define COLOR_LIGHT_RED    "\033[1;31m"
#define COLOR_GREEN        "\033[0;32;32m"
#define COLOR_LIGHT_GREEN  "\033[1;32m"
#define COLOR_BLUE         "\033[0;32;34m"
#define COLOR_LIGHT_BLUE   "\033[1;34m"
#define COLOR_DARY_GRAY    "\033[1;30m"
#define COLOR_CYAN         "\033[0;36m"
#define COLOR_LIGHT_CYAN   "\033[1;36m"
#define COLOR_PURPLE       "\033[0;35m"
#define COLOR_LIGHT_PURPLE "\033[1;35m"
#define COLOR_BROWN        "\033[0;33m"
#define COLOR_YELLOW       "\033[1;33m"
#define COLOR_LIGHT_GRAY   "\033[0;37m"
#define COLOR_WHITE        "\033[1;37m"

#define DEBUG_ON	   1

#if DEBUG_ON
#define debug_print(msg) {\
	fprintf(stdout, "[PID:%d][%s][%s]:%d %s\n", getpid(), MODULE_NAME, __FUNCTION__, __LINE__, msg); \
}
#else
#define debug_print(msg)
#endif

#if DEBUG_ON
#define PRINTF(first...) { \
	printf(first); \
}
#else
#define PRINTF(first...)
#endif

#if DEBUG_ON
#define LOG_STD_INFO(std, first...) {\
	fprintf(std, "[PID:%d][%s][%s]:%d ", getpid(), MODULE_NAME, __FUNCTION__, __LINE__);\
	fprintf(std, first);\
	fflush(std);\
}
#else
#define LOG_STD_INFO(std, first...)
#endif

#if DEBUG_ON
#define LOG_NORMAL_INFO(first...) {\
    printf("[PID:%d]" COLOR_NONE_C "[%s][%s]:%d ", getpid(), MODULE_NAME, __FUNCTION__, __LINE__);\
    printf(first);\
    fflush(stdout);\
}
#else
#define LOG_NORMAL_INFO(first...)
#endif

#if DEBUG_ON
#define LOG_ERROR_INFO(first...) {\
    printf("[PID:%d]" COLOR_RED "[%s][%s][Error]:%d ", getpid(), MODULE_NAME, __FUNCTION__, __LINE__);\
    printf(first);\
	printf(COLOR_NONE_C);\
    fflush(stdout);\
}
#else
#define LOG_ERROR_INFO(first...)
#endif

#if DEBUG_ON
#define LOG_PERROR_INFO(first...) {\
    printf("[PID:%d]" COLOR_RED "[%s][%s][perror]:%d ", getpid(), MODULE_NAME, __FUNCTION__, __LINE__);\
    printf(COLOR_RED first);\
    printf(COLOR_RED " errno = %d: %s\n", errno, strerror(errno));\
	printf(COLOR_NONE_C);\
    fflush(stdout);\
}
#else
#define LOG_PERROR_INFO(first...)
#endif

#if DEBUG_ON
#define LOG_WARN_INFO(first...) {\
    printf("[PID:%d]" COLOR_YELLOW "[%s][%s][Warn]:%d ", getpid(), MODULE_NAME, __FUNCTION__, __LINE__);\
    printf(first);\
	printf(COLOR_NONE_C);\
    fflush(stdout);\
}
#else
#define LOG_WARN_INFO(first...)
#endif

#if DEBUG_ON
#define LOG_HL_INFO(first...) {\
    printf("[PID:%d]" COLOR_GREEN "[%s][%s][HL]:%d ", getpid(), MODULE_NAME, __FUNCTION__, __LINE__);\
    printf(first);\
	printf(COLOR_NONE_C);\
    fflush(stdout);\
}
#else
#define LOG_HL_INFO(first...)
#endif

#if DEBUG_ON
#define LOG_DC_INFO(color, first...) {\
    printf("[PID:%d]" color "[%s][%s][HL]:%d ", getpid(), MODULE_NAME, __FUNCTION__, __LINE__);\
    printf(first);\
	printf(COLOR_NONE_C);\
    fflush(stdout);\
}
#else
#define LOG_DC_INFO(color, first...)
#endif

#if DEBUG_ON
#define LOG_DATA_INFO(pbyData, iLen, iMod, first...) { \
	int i;\
    printf("[PID:%d]" COLOR_NONE_C "[%s][%s]:%d ", getpid(), MODULE_NAME, __FUNCTION__, __LINE__);\
    printf(first);\
    for (i = 0; i < iLen; i++) \
	{ \
    	printf("%02X ", pbyData[i]); \
	} \
	printf("\n");\
	fflush(stdout);\
}
#else
#define LOG_DATA_INFO(pbyData, iLen, iMod, first...)
#endif

/*****************END****************/




#endif




