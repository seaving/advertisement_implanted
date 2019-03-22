#include "includes.h"

int create_server()
{
	if (init_wifidog_msg_html(DEFAULT_INTERNET_OFFLINE_FILE, DEFAULT_AUTHSERVER_OFFLINE_FILE) < 0
	|| init_wifidog_redir_html(DEFAULT_REDIRECTFILE) < 0
	|| init_wifidog_result_msg_html(DEFAULT_HTMLMSGFILE) < 0)
	{
		LOG_ERROR_INFO("init html page error.\n");
		exit(-1);
	}
	
	tpool_create(30);

#if 1
	pthread_t thd;
	if (pthread_create(&thd, NULL, _create_http_redirect_server_, NULL) != 0)
	{
		LOG_PERROR_INFO("pthread_create failt!");
		return -1;
	}
#endif

	pthread_t thd1;
	if (pthread_create(&thd1, NULL, _create_https_redirect_server_, NULL) != 0)
	{
		LOG_PERROR_INFO("pthread_create failt!");
		return -1;
	}

	pthread_t thd2;
	if (pthread_create(&thd2, NULL, _create_auth_server_, NULL) != 0)
	{
		LOG_PERROR_INFO("pthread_create failt!");
		return -1;
	}

#if 0
	pthread_t thd3;
	if (pthread_create(&thd3, NULL, _create_web_server_, NULL) != 0)
	{
		LOG_PERROR_INFO("pthread_create failt!");
		return -1;
	}
#endif

	return 0;
}


