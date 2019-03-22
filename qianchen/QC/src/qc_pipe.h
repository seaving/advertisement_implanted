#ifndef __QC_PIPE_H__
#define __QC_PIPE_H__

#include "qc_pipe.h"

bool qc_pipe_create(int *rfd, int *wfd);
void qc_pipe_destroy(int rfd, int wfd);
int qc_pipe_write(int wfd, char *data, int data_len);
int qc_pipe_read(int rfd, char *buf, int read_len);



#endif

