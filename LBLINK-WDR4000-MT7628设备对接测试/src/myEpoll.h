#ifndef __MY_EPOLL_H_
#define __MY_EPOLL_H_


int epoll_init(int listendfd);
int epoll_add_sockfd(int epollfd, int sockfd, int mode, void *ev_data_ptr);
int epoll_del_sockfd(int epollfd, int sockfd);
int epoll_mod_sockfd(int epollfd, int sockfd, int mode, void *ev_data_ptr);

#endif

