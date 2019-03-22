#ifndef __EPOLL_LISTEN_H__
#define __EPOLL_LISTEN_H__	1

extern int g_connect_count;

void *listen_thread(void *arg);
int epoll_create_listen(int listenfd);
int epoll_wait_listen(int epollfd, int listenfd);

int listen_thread_start(int listenfd);
int display_client_counts();

#endif

