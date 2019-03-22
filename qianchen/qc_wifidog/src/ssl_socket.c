#include "includes.h"

static void *my_zeroing_malloc(size_t howmuch)
{
	return calloc(1, howmuch);
}

void common_setup (void)
{
	CRYPTO_set_mem_functions(my_zeroing_malloc, realloc, free);
	SSL_library_init();
	SSL_load_error_strings();
	OpenSSL_add_all_algorithms();

	LOG_NORMAL_INFO("Using OpenSSL version \"%s\"\nand libevent version \"%s\"\n",
 			SSLeay_version(SSLEAY_VERSION), event_get_version());
}

#if 0
SSL_CTX *ssl_init()
{
	ERR_load_BIO_strings();
	
    /* SSL 库初始化 */
    SSL_library_init();
    
    /* 载入所有 SSL 算法 */
    OpenSSL_add_all_algorithms();
    
    /* 载入所有 SSL 错误消息 */
    SSL_load_error_strings();
    
    /* 以 SSL V2 和 V3 标准兼容方式产生一个 SSL_CTX ，即 SSL Content Text */
    SSL_CTX *ctx = SSL_CTX_new(SSLv23_server_method());
    
    /* 也可以用 SSLv2_server_method() 或 SSLv3_server_method() 单独表示 V2 或 V3标准 */
    if (ctx == NULL)
    {
        ERR_print_errors_fp(stdout);
        return NULL;
    }
    
    /* 载入用户的数字证书， 此证书用来发送给客户端。 证书里包含有公钥 */
    if (SSL_CTX_use_certificate_file(ctx, CA_FILE, SSL_FILETYPE_PEM) <= 0)
    {
        ERR_print_errors_fp(stdout);
        ssl_ctx_free(ctx);
        return NULL;
    }
    
    /* 载入用户私钥 */
    if (SSL_CTX_use_PrivateKey_file(ctx, PRIVATE_FILE, SSL_FILETYPE_PEM) <= 0)
    {
        ERR_print_errors_fp(stdout);
        ssl_ctx_free(ctx);
        return NULL;
    }
    
    /* 检查用户私钥是否正确 */
    if (! SSL_CTX_check_private_key(ctx))
    {
        ERR_print_errors_fp(stdout);
        ssl_ctx_free(ctx);
        return NULL;
    }
    
	return ctx;
}
#endif

void ssl_close(SSL *ssl)
{
	if (ssl)
	{
		/* 关闭 SSL 连接 */
		SSL_shutdown(ssl);
		
		/* 释放 SSL */
		SSL_free(ssl);

		ssl = NULL;
	}
}

void ssl_ctx_free(SSL_CTX *ctx)
{
    /* 释放 CTX */
    if (ctx)
    {
    	SSL_CTX_free(ctx);
	}
	
	ctx = NULL;
}

int ssl_socket_read(SSL *ssl, char *buf, int readlen, unsigned char time_out)
{
    int count = 0;
    int ret = -1;
    unsigned int last_time = SYSTEM_SEC;
    
	for ( ; ; )
    {    	
		if (time_out > 0 && SYSTEM_SEC - last_time > time_out)
		{
			return -1;
		}
		
		count = SSL_read(ssl, buf, readlen);
		ret = SSL_get_error(ssl, count);
		if (ret == SSL_ERROR_NONE)
		{
			if (count > 0)
			{
				//printf("count = %d\n", count);
				//重新赋值开始计算时间
				last_time = SYSTEM_SEC;
				return count;
			}
		}
		else if (ret == SSL_ERROR_WANT_READ)
		{
			usleep(100);
			continue;
		}
		else
		{
			LOG_ERROR_INFO("ssl read error!\n");
			return -1;
		}
    }

    return -1;
}

int ssl_socket_send(SSL *ssl, char *buf, int len, unsigned char time_out)
{
    int count = 0;
    int ret = -1;
    int offset = 0;
    int send_len = len;
    unsigned int last_time = SYSTEM_SEC;
    
    while (send_len > 0)
    {
		if (time_out > 0 && SYSTEM_SEC - last_time > time_out)
		{
			return -1;
		}
		
		count = SSL_write(ssl, buf + offset, send_len);
		ret = SSL_get_error(ssl, count);
		if (ret == SSL_ERROR_NONE)
		{
			if (count > 0)
			{
				offset += count;
				send_len -= count;
				last_time = SYSTEM_SEC;
			}
		}
		else if (ret == SSL_ERROR_WANT_READ /*SSL_ERROR_WANT_WRITE*/)
		{
			usleep(100);
			continue;
		}
		else
		{
			LOG_ERROR_INFO("ssl send error!\n");
			return -1;
		}
    }

	if (send_len == 0)
		return send_len;
	
    return -1;
}

SSL *ssl_accept(SSL_CTX *ctx, int fd, int time_out)
{
	SSL *ssl = NULL;

	/* 基于 ctx 产生一个新的 SSL */
	ssl = SSL_new(ctx);
	if (! ssl)
	{
		LOG_ERROR_INFO("SSL_new failt!\n");
		return NULL;
	}
	
	/* 将连接用户的 socket 加入到 SSL */
	SSL_set_fd(ssl, fd);
	
	unsigned int last_time = SYSTEM_SEC;
	bool isContinue = true;
	while (isContinue)
	{
		isContinue = false;
		
		if (time_out > 0 && SYSTEM_SEC - last_time > time_out)
		{
			ssl_close(ssl);
			return NULL;
		}

		/* 建立SSL连接 */
		int ret = SSL_accept(ssl);
		if (ret != 1)
		{
			ret = SSL_get_error(ssl, -1);
			if (ret == SSL_ERROR_WANT_READ)
			{
				isContinue = true;
			}
			else
			{
				LOG_PERROR_INFO("--- SSL accept failt! ");
				ssl_close(ssl);
				return NULL;
			}	
		}
		else
		{
			break;
		}
	}

	return ssl;
}

