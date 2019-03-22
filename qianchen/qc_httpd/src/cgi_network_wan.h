#ifndef __CGI_NETWORK_WAN_H__
#define __CGI_NETWORK_WAN_H__



int cgi_get_wan_protocol(struct evhttp_request *req, const t_http_server *http_server);
int cgi_get_wan_info(struct evhttp_request *req, const t_http_server *http_server);

int cgi_set_wan(struct evhttp_request *req, const t_http_server *http_server);


#endif


