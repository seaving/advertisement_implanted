#ifndef __AUTH_H__
#define __AUTH_H__

#define AUTH_ERROR		'0'		//错误
#define AUTH_ALLOW		'1'		//认证成功
#define AUTH_REDIRECTED 'r'		//认证成功并且跳转到指定页面
#define AUTH_SNIFF  	'2'

char auth(const char *path, const char *uri, char *portal, int *temp_accept_time);
void *_create_auth_server_(void *arg);

#endif

