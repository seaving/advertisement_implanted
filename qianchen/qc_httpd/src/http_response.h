
#ifndef __HTTP_RESPONSE_H__
#define __HTTP_RESPONSE_H__
#if 0
typedef enum
{
	E_RES_200 = 200,
	E_RES_302 = 302,
	E_RES_404 = 404,
	E_RES_END
} resState_t;


bool send_response_302(int client_sock, char * to_url);
bool send_response_404(int client_sock);
bool send_response_500(int client_sock);
bool http_response_process(int client_sock, char *path, char *cookie, char *post_data);
bool send_response_with_cookie_302(int client_sock, char *to_url, char *cookie);
#endif

#endif

