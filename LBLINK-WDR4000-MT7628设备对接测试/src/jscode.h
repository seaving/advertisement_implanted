#ifndef __JS_CODE_H__
#define __JS_CODE_H__


extern int g_adHostLen;
extern char g_adHost[512];

extern int g_adPort;

extern int g_adStrPortLen;
extern char g_adStrPort[33];


char *get_js_code_h();
char *get_js_code_e();
int get_js_code_h_len();
int get_js_code_e_len();
int get_js_code_from_server();
void free_js();
int js_add_param(char *param_name, char *param_arg, char *addr);
int struct_js_parma(int sockfd, char *js, char *url, int url_len, char *title, int title_len, char *keyword, int keyword_len);
int js_add_param_n(char *param_name, int param_name_len, char *param_arg, int param_arg_len, char *addr);

#endif
