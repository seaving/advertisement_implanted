
#ifndef __USER_SSL_SOCKET_H__
#define __USER_SSL_SOCKET_H__

#include "common.h"

ssize_t ssl_readline(SSL *ssl, void *usrbuf, size_t maxlen);

#endif

