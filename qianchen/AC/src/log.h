#ifndef __LOG_H__
#define __LOG_H__

#include "common.h"

#define MAX_LOG_LINE_LENGTH (2048)
typedef struct _LOG
{
	int fd;
	char buf[MAX_LOG_LINE_LENGTH];
} LOG;

typedef struct _LOG_QUEUE
{
	uint32 size;
	uint32 producer;
	uint32 consumer;
	LOG items[0];
} LOG_QUEUE;

LOG_QUEUE* create_log_queue();

sint32 record_log(LOG_QUEUE *queue, int fd, const char *fmt, ...);

sint32 start_log_thread();

#define slog_err_t_w(queue, fmt, args...) record_log(queue, STDERR_FILENO, "[%ld] [ERROR] [%s] "fmt"\n", (long)time(NULL), __func__, ##args);
#define slog_err_t(queue, fmt, args...) record_log(queue, STDERR_FILENO, "[%ld] [ERROR] "fmt"\n", (long)time(NULL), ##args);
#define slog_err(queue, fmt, args...) record_log(queue, STDERR_FILENO, "[ERROR] "fmt"\n", ##args);
#define slog_info_t_w(queue, fmt, args...) record_log(queue, STDOUT_FILENO, "[%ld] [INFO] [%s] "fmt"\n", (long)time(NULL), __func__, ##args);
#define slog_info_t(queue, fmt, args...) record_log(queue, STDOUT_FILENO, "[%ld] [INFO] "fmt"\n", (long)time(NULL), ##args);
#define slog_info(queue, fmt, args...) record_log(queue, STDOUT_FILENO, "[INFO] "fmt"\n", ##args);
#define slog_warn_t_w(queue, fmt, args...) record_log(queue, STDOUT_FILENO, "[%ld] [WARN] [%s] "fmt"\n", (long)time(NULL), __func__, ##args);
#define slog_warn_t(queue, fmt, args...) record_log(queue, STDOUT_FILENO, "[%ld] [WARN] "fmt"\n", (long)time(NULL), ##args);
#define slog_warn(queue, fmt, args...) record_log(queue, STDOUT_FILENO, "[WARN] "fmt"\n", ##args);

#endif

