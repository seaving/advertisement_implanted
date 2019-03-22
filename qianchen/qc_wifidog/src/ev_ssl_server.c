#include "includes.h"

typedef struct _https_server_t
{
	char	*ca_crt_file;
	char	*svr_crt_file;
	char	*svr_key_file;
	short	gw_https_port;
} t_https_server;

typedef struct _popular_server_t
{
    char *hostname;
    struct _popular_server_t *next;
} t_popular_server;

static struct event_base *base		= NULL;
//static struct evdns_base *dnsbase 	= NULL;

#define die_most_horribly_from_openssl_error(func) { \
	LOG_ERROR_INFO("%s failed:\n", func); \
	exit (EXIT_FAILURE); \
}

/*static void server_setup_certs(SSL_CTX *ctx, const char *certificate_chain, const char *private_key)
{ 
  	if (1 != SSL_CTX_use_certificate_chain_file(ctx, certificate_chain))
    	die_most_horribly_from_openssl_error ("SSL_CTX_use_certificate_chain_file");

  	if (1 != SSL_CTX_use_PrivateKey_file(ctx, private_key, SSL_FILETYPE_PEM))
    	die_most_horribly_from_openssl_error("SSL_CTX_use_PrivateKey_file");

  	if (1 != SSL_CTX_check_private_key(ctx))
    	die_most_horribly_from_openssl_error("SSL_CTX_check_private_key");
}*/

/*static void check_internet_available_cb(int errcode, struct evutil_addrinfo *addr, void *ptr)
{

}*/

/*static void check_internet_available(t_popular_server *popular_server)
{
	if (! popular_server)
		return;

	struct evutil_addrinfo hints;
	memset(&hints, 0, sizeof(hints));

    hints.ai_family = AF_UNSPEC;
    hints.ai_flags = EVUTIL_AI_CANONNAME;
    hints.ai_socktype = SOCK_STREAM;
    hints.ai_protocol = IPPROTO_TCP;
	
	evdns_getaddrinfo(dnsbase, popular_server->hostname, NULL,
          &hints, check_internet_available_cb, popular_server);
	
}*/

static struct bufferevent* bevcb(struct event_base *base, void *arg)
{ 
	struct bufferevent* r;
  	SSL_CTX *ctx = (SSL_CTX *)arg;

  	r = bufferevent_openssl_socket_new(base, -1,
                                      SSL_new (ctx),
                                      BUFFEREVENT_SSL_ACCEPTING,
                                      BEV_OPT_CLOSE_ON_FREE);
  	return r;
}

void server_setup_certs (SSL_CTX *ctx,
                            const char *certificate_chain,
                            const char *private_key)
{
  	if (1 != SSL_CTX_use_certificate_chain_file (ctx, certificate_chain))
    	die_most_horribly_from_openssl_error ("SSL_CTX_use_certificate_chain_file");

  	if (1 != SSL_CTX_use_PrivateKey_file (ctx, private_key, SSL_FILETYPE_PEM))
    	die_most_horribly_from_openssl_error ("SSL_CTX_use_PrivateKey_file");

  	if (1 != SSL_CTX_check_private_key (ctx))
    	die_most_horribly_from_openssl_error ("SSL_CTX_check_private_key");
}

int https_redirect(char *gw_ip, t_https_server *https_server)
{
  	struct evhttp *http;
  	struct evhttp_bound_socket *handle;
	//struct event timeout;
	//struct timeval tv;

	base = event_base_new();
  	if (! base)
  	{
		LOG_PERROR_INFO("event_base_new failt.");
      	return -1;
    }
	
  	/* Create a new evhttp object to handle requests. */
  	http = evhttp_new(base);
  	if (! http)
  	{
		LOG_PERROR_INFO("evhttp_new failt.");
      	goto err;
    }

#if 1
	ERR_load_BIO_strings();
	
    /* SSL 库初始化 */
    SSL_library_init();
    
    /* 载入所有 SSL 算法 */
    OpenSSL_add_all_algorithms();
    
    /* 载入所有 SSL 错误消息 */
    SSL_load_error_strings();

 	SSL_CTX *ctx = SSL_CTX_new(SSLv23_server_method());
	if (! ctx)
		die_most_horribly_from_openssl_error("ssl_init");
 	
  	SSL_CTX_set_options(ctx,
                       SSL_OP_SINGLE_DH_USE |
                       SSL_OP_SINGLE_ECDH_USE |
                       SSL_OP_NO_SSLv2);

	/* Cheesily pick an elliptic curve to use with elliptic curve ciphersuites.
	* We just hardcode a single curve which is reasonably decent.
	* See http://www.mail-archive.com/openssl-dev@openssl.org/msg30957.html */
	EC_KEY *ecdh = EC_KEY_new_by_curve_name(NID_X9_62_prime256v1);
	if (! ecdh)
    	die_most_horribly_from_openssl_error("EC_KEY_new_by_curve_name");
  	if (1 != SSL_CTX_set_tmp_ecdh(ctx, ecdh))
    	die_most_horribly_from_openssl_error("SSL_CTX_set_tmp_ecdh");

	server_setup_certs(ctx, https_server->svr_crt_file, https_server->svr_key_file);
#else
	SSL_CTX *ctx = ssl_init();
	if (! ctx)
		die_most_horribly_from_openssl_error("ssl_init");
#endif

	/* This is the magic that lets evhttp use SSL. */
	evhttp_set_bevcb(http, bevcb, ctx);
 
	/* This is the callback that gets called when a request comes in.*/
	int *arg = calloc(sizeof(int), 1);
	if (arg)
		*arg = QC_DOG_HTTPS_REDIRECT_SERVER_PORT;
	evhttp_set_gencb(http, redirect_cb, arg);

	/* Now we tell the evhttp what port to listen on */
	handle = evhttp_bind_socket_with_handle(http, gw_ip, https_server->gw_https_port);
	if (! handle)
	{ 
		LOG_PERROR_INFO("couldn't bind to port %d .",
               (int) https_server->gw_https_port);
		goto err;
    }

    event_base_dispatch(base);

err:
	ssl_ctx_free(ctx);
	//event_del(&timeout);

	if (http && handle)
		evhttp_del_accept_socket(http, handle);
	if (http)
		evhttp_free(http);
	if (base)
		event_base_free(base);
	//if (dnsbase)
		//evdns_base_free(dnsbase, 0);

	if (arg)
		free(arg);
  	/* not reached; runs forever */
  	return -1;
}

int ssl_server_listen_port()
{
	return QC_DOG_HTTPS_REDIRECT_SERVER_PORT;
}

static int ssl_server_create()
{
	t_https_server https_server;
	https_server.gw_https_port = QC_DOG_HTTPS_REDIRECT_SERVER_PORT;
	https_server.ca_crt_file	= safe_strdup(DEFAULT_CA_CRT_FILE);
	https_server.svr_crt_file	= safe_strdup(DEFAULT_SVR_CRT_FILE);
	https_server.svr_key_file	= safe_strdup(DEFAULT_SVR_KEY_FILE);

	char lan_ip[33] = {0};
	get_dev_ip(lan_ip, GET_LAN_NAME);
	https_redirect(lan_ip, &https_server);

	free_malloc(https_server.ca_crt_file);
	free_malloc(https_server.svr_crt_file);
	free_malloc(https_server.svr_key_file);

    return -1;
}

void *_create_https_redirect_server_(void *arg)
{
	pthread_detach(pthread_self());

	for ( ; ; )
	{
		ssl_server_create();

		sleep(1);
	}

	return NULL;
}


