#ifndef __HTTP_FILTER_H__
#define __HTTP_FILTER_H__


typedef enum
{
	E_HTTP_FILTER_REDICRECT_LOGIN = 0,
	E_HTTP_FILTER_REDICRECT_HOME,
	E_HTTP_FILTER_NORMAL,
	E_HTTP_FILTER_ERROR,
	E_HTTP_FILTER_END_TALK,
	E_HTTP_FILTER_REDICRECT_SET_PASSWD
} http_filter_t;

#define SESSION_VALIDITY	10 * 60


void s_cookie_init();
void s_cookie_password_set(char *password);

http_filter_t http_filter(char *path, char *cookie, char *post_data);
void s_cookie_set(char *sesson_name, char *session_id, int start, int validity, bool flg);

bool cookie_check(char *cookie);
bool passwd_check(char *password);

void create_cookie(char *cookie);


#endif



