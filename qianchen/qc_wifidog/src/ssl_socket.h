#ifndef __SSL_SOCKET_H__
#define __SSL_SOCKET_H__

#include "common.h"

#if 0
SSL_CTX *ssl_init();
#endif

void common_setup(void);

SSL *ssl_accept(SSL_CTX *ctx, int fd, int time_out);

void ssl_close(SSL *ssl);

void ssl_ctx_free(SSL_CTX *ctx);

int ssl_socket_read(SSL *ssl, char *buf, int readlen, unsigned char time_out);

int ssl_socket_send(SSL *ssl, char *buf, int len, unsigned char time_out);

#endif


