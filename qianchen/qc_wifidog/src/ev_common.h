#ifndef __EV_COMMON_H__
#define __EV_COMMON_H__

#include "common.h"

void print_request_head_info(struct evkeyvalq *header);

void redirect_cb(struct evhttp_request *req, void *arg);
char *arp_get(const char *req_ip);
char *evhttpd_get_full_redir_url(const char *mac, const char *ip, const char *mobile_name, const char *orig_url);
char *evhttp_get_request_encode_url(struct evhttp_request *req, int port);
char *evhttp_get_request_url(struct evhttp_request *req, int port);

void *start_http_requset(struct event_base* base, const char *url, int req_get_flag,
                 const char *content_type, const char* data);
                 
void evhttp_gw_reply_js_redirect(struct evhttp_request *req, const char *redirect_url);
void evhttp_gw_reply_302_redirect(struct evhttp_request *req, const char *redirect_url);
void evhttp_reply_sniff(struct evhttp_request *req,const char *url);

char *safe_strdup(const char *s);
void *safe_malloc(size_t size);

int init_wifidog_msg_html(char *internet_offline_file, char *authserver_offline_file);
int init_wifidog_redir_html(char *htmlredirfile);
int init_wifidog_result_msg_html(char *htmlresmsgfile);

void page_release();

void evhttp_reply_response_connection_closed(struct evhttp_request *req, int res_code, char *state, char *titile, char *msg);

#endif

