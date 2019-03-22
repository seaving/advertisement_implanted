#ifndef __JS_CODE_H__
#define __JS_CODE_H__

typedef struct __ad_js_
{
	int adHostLen;
	char *adHost;
	int js_len;
	char *js;
	int script_id_len;
	char *script_id;
	int script_h_len;
	char *script_h;
	int script_m_len;
	char *script_m;
	int script_e_len;
	char *script_e;
	int adPort;
	int adStrPortLen;
	char *adStrPort;
	char *jsUrl_mark;
	int jsUrl_mark_len;
	char *adUrl_mark;
	int adUrl_mark_len;
} adjs_t;

extern adjs_t adjs;

#define GET_JS_JSON_PARMA_BUF_SIZE(url_len)	(url_len + DEVNO_BUF_SIZE + 33 + 33 + 1)

int get_js_code_from_server();
void free_adjs();
int js_add_param(char *param_name, char *param_arg, char *addr);
int struct_js_parma(int sockfd, char *js, char *url, int url_len, char *title, int title_len, char *keyword, int keyword_len);
int struct_js_json_parma(int sockfd, char *js, int size, char *url, int url_len);
int js_add_param_n(char *param_name, int param_name_len, char *param_arg, int param_arg_len, char *addr);
int struct_js_aes_parma(int sockfd, char *js, int size, char *url, int url_len);

#endif
