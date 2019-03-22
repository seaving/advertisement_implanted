#include "includes.h"
#if 1
struct event_base *g_evhttp_base;

static int http_redirect(const char *address, const t_http_server *http_server)
{
	struct evhttp *http;
	struct evhttp_bound_socket *handle;

	g_evhttp_base = event_base_new();
	if (! g_evhttp_base)
	{
		LOG_PERROR_INFO("event_base_new failt !\n");
		goto end_loop;
	}

	/* Create a new evhttp object to handle requests. */
	http = evhttp_new(g_evhttp_base);
	if (! http)
	{
		goto end_loop;
	}

	int *arg = calloc(sizeof(int), 1);
	if (arg)
		*arg = QC_DOG_HTTP_REDIRECT_SERVER_PORT;

	//evhttp_set_cb(http, "/", test, NULL);
	evhttp_set_gencb(http, redirect_cb, arg);

	handle = evhttp_bind_socket_with_handle(http, address, http_server->gw_http_port);
	if (!handle)
	{
		goto end_loop;
	}

	event_base_dispatch(g_evhttp_base);

end_loop:
	if (handle)
		evhttp_del_accept_socket(http, handle);

	if (http)
		evhttp_free(http);

	if (g_evhttp_base)
		event_base_free(g_evhttp_base);

	if (arg)
		free(arg);

	return -1;
}

int server_listen_port()
{
	return QC_DOG_HTTP_REDIRECT_SERVER_PORT;
}

void *_create_http_redirect_server_(void *arg)
{
	pthread_detach(pthread_self());

	t_http_server http_server;

	for ( ; ; )
	{
		http_server.gw_http_port = QC_DOG_HTTP_REDIRECT_SERVER_PORT;

		char lan_ip[33] = {0};
		get_dev_ip(lan_ip, GET_LAN_NAME);
		http_redirect(lan_ip, &http_server);

		sleep(1);
	}

	return NULL;
}

#endif

#if 0
#include <netinet/in.h>  
#include <sys/socket.h>  
#include <fcntl.h>  
  
#include <event2/event.h>  
#include <event2/buffer.h>  
#include <event2/bufferevent.h>  
  
#include <assert.h>  
#include <unistd.h>  
#include <string.h>  
#include <stdlib.h>  
#include <stdio.h>  
#include <errno.h>  
  
void do_read(evutil_socket_t fd, short events, void *arg);  
  
//struct bufferevent内建了两个event(read/write)和对应的缓冲区(struct evbuffer *input, *output)，并提供相应的函数用来操作>  
//缓冲区(或者直接操作bufferevent)  
//接收到数据后，判断是不一样一条消息的结束，结束标志为"over"字符串  
void readcb(struct bufferevent *bev, void *ctx)  
{
    printf("called readcb!\n");
    struct evbuffer *input, *output;
    char *request_line;
    size_t len;

    //其实就是取出bufferevent中的input
    input = bufferevent_get_input(bev);

    //其实就是取出bufferevent中的output
    output = bufferevent_get_output(bev);

    size_t input_len = evbuffer_get_length(input);
    LOG_NORMAL_INFO("input_len: %d\n", input_len);
    
    size_t output_len = evbuffer_get_length(output);
    LOG_NORMAL_INFO("output_len: %d\n", output_len);

	for ( ; ; )
    {
    	//从evbuffer前面取出一行，用一个新分配的空字符结束 
		//的字符串返回这一行,EVBUFFER_EOL_CRLF表示行尾是一个可选的回车，后随一个换行符  
        request_line = evbuffer_readln(input, &len, EVBUFFER_EOL_CRLF_STRICT); 
        if(NULL == request_line)
        {
            LOG_WARN_INFO("The line has not arrived yet.\n");

            //之所以要进行free是因为 line = mm_malloc(n_to_copy+1))，在这里进行了malloc
            //free(request_line);
            break;
        }
        else
		{
            LOG_HL_INFO("%d : %s", len, request_line);

            //用于判断是不是一条消息的结束
            if(strcmp(request_line, "\r\n") == 0)
            {
                char *response = "HTTP/1.1 200 OK\r\nContent-length: 3\r\n\r\n ok";
                evbuffer_add(output, response, strlen(response));
                free(request_line);
                break;
            }
            
            free(request_line);
        }
    }

    size_t input_len1 = evbuffer_get_length(input);
    LOG_NORMAL_INFO("input_len1: %d\n", input_len1);
    
    size_t output_len1 = evbuffer_get_length(output);
    LOG_NORMAL_INFO("output_len1: %d\n\n", output_len1);
}

void errorcb(struct bufferevent *bev, short error, void *ctx)
{
    if (error & BEV_EVENT_EOF)
    {
        /* connection has been closed, do any clean up here */  
        printf("connection closed\n");
    }
    else if (error & BEV_EVENT_ERROR)
    {
        /* check errno to see what error occurred */
        printf("some other error\n");
    }
    else if (error & BEV_EVENT_TIMEOUT)
    {
        /* must be a timeout event handle, handle it */
        printf("Timed out\n");
    }
    
    bufferevent_free(bev);
}

void do_accept(evutil_socket_t listener, short event, void *arg)
{
    struct event_base *base = arg;
    struct sockaddr_storage ss;
    socklen_t slen = sizeof(ss);
    int fd = accept(listener, (struct sockaddr*)&ss, &slen);
    if (fd < 0)
    {
        perror("accept");
    }
    else if (fd > FD_SETSIZE)
    {
        close(fd);
    }
    else  
    {
        struct bufferevent *bev;
        evutil_make_socket_nonblocking(fd);

        //使用bufferevent_socket_new创建一个struct bufferevent *bev，关联该sockfd，托管给event_base
        ////BEV_OPT_CLOSE_ON_FREE表示释放bufferevent时关闭底层传输端口。这将关闭底层套接字，释放底层bufferevent等。
        bev = bufferevent_socket_new(base, fd, BEV_OPT_CLOSE_ON_FREE);

        //设置读写对应的回调函数
        bufferevent_setcb(bev, readcb, NULL, errorcb, NULL);
  
        //启用读写事件,其实是调用了event_add将相应读写事件加入事件监听队列poll。
        //正如文档所说，如果相应事件不置为true，bufferevent是不会读写数据的  
        bufferevent_enable(bev, EV_READ | EV_WRITE);
    }
}

int evhttp_server_create()
{
    evutil_socket_t listener;
    struct event_base *base;
    struct event *listener_event;
    
    listener = socket_listen(NULL, 65000, 300);
    if (listener < 0)
    {
		LOG_ERROR_INFO("socket_listen failt !\n");
		return -1;
    }
    evutil_make_socket_nonblocking(listener);

    base = event_base_new();
    if (! base)
    {
    	LOG_PERROR_INFO("event_base_new error.");
    	socket_close(listener);
        return -1;
	}
	
    listener_event = event_new(base, listener, EV_READ | EV_PERSIST, do_accept, (void*)base);
	if (! listener_event)
	{
		LOG_PERROR_INFO("event_new error.");
		event_base_free(base);
		return -1;
	}

    event_add(listener_event, NULL);

    event_base_dispatch(base);

    return 0;
}

#endif
#if 0
#include <sys/queue.h>

//for http
#include <evhttp.h>

#define MYHTTPD_SIGNATURE   "myhttpd v 0.0.1"


//当向进程发出SIGTERM/SIGHUP/SIGINT/SIGQUIT的时候，终止event的事件侦听循环
void signal_handler(int sig)
{
    switch (sig)
    {
        case SIGTERM:
        case SIGHUP:
        case SIGQUIT:
        case SIGINT:
		{
            event_loopbreak();
            exit(0);
            break;
        }
        case SIGPIPE:
        {
        	break;
        }
    }
}

//处理模块
void httpd_handler(struct evhttp_request *req, void *arg)
{
	char output[2048] = {0};
	char tmp[1024] = {0};

	//获取客户端请求的URI(使用evhttp_request_uri或直接req->uri)
	const char *uri;
	uri = evhttp_request_uri(req);
	sprintf(tmp, "uri=%s\n", uri);
	strcat(output, tmp);

	sprintf(tmp, "uri=%s\n", req->uri);
	strcat(output, tmp);

	//decoded uri
	char *decoded_uri;
	decoded_uri = evhttp_decode_uri(uri);
	sprintf(tmp, "decoded_uri=%s\n", decoded_uri);
	strcat(output, tmp);

	//解析URI的参数(即GET方法的参数)
	struct evkeyvalq params;
	evhttp_parse_query(decoded_uri, &params);
	sprintf(tmp, "q=%s\n", evhttp_find_header(&params, "q"));
	strcat(output, tmp);
	sprintf(tmp, "s=%s\n", evhttp_find_header(&params, "s"));
	strcat(output, tmp);
	free(decoded_uri);

	//获取POST方法的数据
	char *post_data = (char *) EVBUFFER_DATA(req->input_buffer);
	sprintf(tmp, "post_data=%s\n", post_data);
	strcat(output, tmp);

	/*
	具体的：可以根据GET/POST的参数执行相应操作，然后将结果输出
	...
	*/

	/* 输出到客户端 */

	//HTTP header
	evhttp_add_header(req->output_headers, "Server", MYHTTPD_SIGNATURE);
	evhttp_add_header(req->output_headers, "Content-Type", "text/plain; charset=UTF-8");
	evhttp_add_header(req->output_headers, "Connection", "close");
	
	//输出的内容
	struct evbuffer *buf;
	buf = evbuffer_new();
	evbuffer_add_printf(buf, "It works!\n%s\n", output);
	evhttp_send_reply(req, HTTP_OK, "OK", buf);
	evbuffer_free(buf);
}

void test(struct evhttp_request *req, void *arg)
{
	httpd_handler(req, arg);
}

int evhttp_server_create()
{
    //自定义信号处理函数
    signal(SIGHUP, signal_handler);
    signal(SIGTERM, signal_handler);
    signal(SIGINT, signal_handler);
    signal(SIGQUIT, signal_handler);
    signal(SIGPIPE, signal_handler);

    //默认参数
    char *httpd_option_listen = "0.0.0.0";
    int httpd_option_port = 8080;
    int httpd_option_timeout = 120; //in seconds

    /* 使用libevent创建HTTP Server */

    //初始化event API
    event_init();

    //创建一个http server
    struct evhttp *httpd;
    httpd = evhttp_start(httpd_option_listen, httpd_option_port);
    evhttp_set_timeout(httpd, httpd_option_timeout);

    //指定generic callback
    //evhttp_set_gencb(httpd, httpd_handler, NULL);
    
    //也可以为特定的URI指定callback
    evhttp_set_cb(httpd, "/", test, NULL);

    //循环处理events
    event_dispatch();

    evhttp_free(httpd);
    
    return 0;
}

#endif

