#include "includes.h"

struct http_request_get
{
    struct evhttp_uri *uri;
    struct event_base *base;
    struct evhttp_connection *cn;
    struct evhttp_request *req;
};

struct http_request_post
{
    struct evhttp_uri *uri;
    struct event_base *base;
    struct evhttp_connection *cn;
    struct evhttp_request *req;
    char *content_type;
    char *post_data;
};

void http_requset_post_cb(struct evhttp_request *req, void *arg);  
void http_requset_get_cb(struct evhttp_request *req, void *arg); 
int start_url_request(struct http_request_get *http_req, int req_get_flag);


void http_requset_post_cb(struct evhttp_request *req, void *arg) 
{
    struct http_request_post *http_req_post = (struct http_request_post *)arg;
    switch (req->response_code)
    {
        case HTTP_OK:
        {
            struct evbuffer* buf = evhttp_request_get_input_buffer(req);
            size_t len = evbuffer_get_length(buf);
            print_request_head_info(req->output_headers);
            
            LOG_HL_INFO("len:%zu  body size:%zu\n", len, req->body_size);       
            char *tmp = malloc(len + 1);
            if (! tmp)
            {
				LOG_PERROR_INFO("malloc error.");
				return ;
            }
            
            memcpy(tmp, evbuffer_pullup(buf, -1), len);
            tmp[len] = '\0';
            LOG_HL_INFO("HTML BODY:%s\n", tmp);
            free(tmp);
            
            event_base_loopexit(http_req_post->base, 0);
            break;
        }
        case HTTP_MOVEPERM:
        {
            LOG_HL_INFO("the uri moved permanently\n");
            break;
		}
        case HTTP_MOVETEMP:
        {
            const char *new_location = evhttp_find_header(req->input_headers, "Location");
            struct evhttp_uri *new_uri = evhttp_uri_parse(new_location);
            evhttp_uri_free(http_req_post->uri);
            http_req_post->uri = new_uri;
            start_url_request((struct http_request_get *)http_req_post, REQUEST_POST_FLAG);
            return;
        }
        default:
        {
            event_base_loopexit(http_req_post->base, 0);
            return;
		}
    }
}

void http_requset_get_cb(struct evhttp_request *req, void *arg)  
{
    struct http_request_get *http_req_get = (struct http_request_get *)arg;
    switch (req->response_code)
    {
        case HTTP_OK:
        {
            struct evbuffer* buf = evhttp_request_get_input_buffer(req);
            size_t len = evbuffer_get_length(buf);
            print_request_head_info(req->output_headers);
            
            LOG_HL_INFO("len:%zu  body size:%zu\n", len, req->body_size);
            char *tmp = malloc(len + 1);
            if (! tmp)
            {
				LOG_PERROR_INFO("malloc error.");
				return ;
            }
            
            memcpy(tmp, evbuffer_pullup(buf, -1), len);
            tmp[len] = '\0';
            LOG_HL_INFO("HTML BODY:%s\n", tmp);
            free(tmp);
            
            event_base_loopexit(http_req_get->base, 0);
            break;
        }
        case HTTP_MOVEPERM:
        {
            LOG_HL_INFO("the uri moved permanently\n");
            break;
		}
        case HTTP_MOVETEMP:
        {
            const char *new_location = evhttp_find_header(req->input_headers, "Location");
            struct evhttp_uri *new_uri = evhttp_uri_parse(new_location);
            evhttp_uri_free(http_req_get->uri);
            http_req_get->uri = new_uri;
            start_url_request(http_req_get, REQUEST_GET_FLAG);
            return;
        }
        default:
        {
            event_base_loopexit(http_req_get->base, 0);
            return;
		}
    }
}

int start_url_request(struct http_request_get *http_req, int req_get_flag)  
{
    if (http_req->cn)
        evhttp_connection_free(http_req->cn);
    
    int port = evhttp_uri_get_port(http_req->uri);
    if (port <= 0) port = 80;
    
    http_req->cn = evhttp_connection_base_new(http_req->base, NULL, evhttp_uri_get_host(http_req->uri), (port == -1 ? 80 : port));  
    if (! http_req->cn)
		return -1;

    /** 
     * Request will be released by evhttp connection 
     * See info of evhttp_make_request() 
     */
    if (req_get_flag == REQUEST_POST_FLAG)
    {
        http_req->req = evhttp_request_new(http_requset_post_cb, http_req);
    }
    else if (req_get_flag ==  REQUEST_GET_FLAG)
    {
        http_req->req = evhttp_request_new(http_requset_get_cb, http_req);
    }

    if (req_get_flag == REQUEST_POST_FLAG)
    {
        const char *path = evhttp_uri_get_path(http_req->uri);
        evhttp_make_request(http_req->cn, http_req->req, EVHTTP_REQ_POST, path ? path : "/");
        
        /** Set the post data */
        struct http_request_post *http_req_post = (struct http_request_post *)http_req;
        evbuffer_add(http_req_post->req->output_buffer, http_req_post->post_data, strlen(http_req_post->post_data));
        evhttp_add_header(http_req_post->req->output_headers, "Content-Type", http_req_post->content_type);
    }
    else if (req_get_flag == REQUEST_GET_FLAG)
    {
        const char *query = evhttp_uri_get_query(http_req->uri);
        const char *path = evhttp_uri_get_path(http_req->uri);
        size_t len = (query ? strlen(query) : 0) + (path ? strlen(path) : 0) + 1;
        char *path_query = NULL;
        if (len > 1)
        {
            path_query = calloc(len, sizeof(char));
            sprintf(path_query, "%s?%s", path, query);
        }
        evhttp_make_request(http_req->cn, http_req->req, EVHTTP_REQ_GET,
                             path_query ? path_query: "/");
    }
    
    /** Set the header properties */
    evhttp_add_header(http_req->req->output_headers, "Host", evhttp_uri_get_host(http_req->uri));

    return 0;  
}  

void print_uri_parts_info(const struct evhttp_uri * http_uri)
{
    LOG_HL_INFO("scheme:%s\n", 		evhttp_uri_get_scheme(http_uri));
    LOG_HL_INFO("host:%s\n", 		evhttp_uri_get_host(http_uri));
    LOG_HL_INFO("path:%s\n", 		evhttp_uri_get_path(http_uri));
    LOG_HL_INFO("port:%d\n", 		evhttp_uri_get_port(http_uri));
    LOG_HL_INFO("query:%s\n", 		evhttp_uri_get_query(http_uri));
    LOG_HL_INFO("userinfo:%s\n",  	evhttp_uri_get_userinfo(http_uri));
    LOG_HL_INFO("fragment:%s\n",  	evhttp_uri_get_fragment(http_uri));
}

void *http_request_new(struct event_base* base, const char *url, int req_get_flag,
                       const char *content_type, const char* data)
{
    int len = 0;
    if (req_get_flag == REQUEST_GET_FLAG)
    {
        len = sizeof(struct http_request_get);
    }
    else if(req_get_flag == REQUEST_POST_FLAG)
    {
        len = sizeof(struct http_request_post);
    }

    struct http_request_get *http_req_get = calloc(1, len);
    if (! http_req_get)
    {
    	LOG_PERROR_INFO("calloc error.");
    	return NULL;
    }
    
    http_req_get->uri = evhttp_uri_parse(url);
    print_uri_parts_info(http_req_get->uri);
    
    http_req_get->base = base;
      
    if (req_get_flag == REQUEST_POST_FLAG)
    {
        struct http_request_post *http_req_post = (struct http_request_post *)http_req_get;  
        if (content_type == NULL)
        {
            content_type = HTTP_CONTENT_TYPE_URL_ENCODED;
        }
        
        http_req_post->content_type = strdup(content_type); 
          
        if (data == NULL)
        {  
            http_req_post->post_data = NULL;
        }
        else
        {
            http_req_post->post_data = strdup(data);
        }
    }

    return http_req_get;
}  

void *start_http_requset(struct event_base* base, const char *url, int req_get_flag,
                 const char *content_type, const char* data)  
{  
    struct http_request_get *http_req_get = http_request_new(base, url, req_get_flag, content_type, data);
    if (! http_req_get)
    {
		LOG_PERROR_INFO("http_request_new error.");
		return NULL;
    }
    
    start_url_request(http_req_get, req_get_flag);
      
    return http_req_get;
}

void http_request_free(struct http_request_get *http_req_get, int req_get_flag)  
{
    evhttp_connection_free(http_req_get->cn);
    evhttp_uri_free(http_req_get->uri);
    if (req_get_flag == REQUEST_GET_FLAG)
    {
        free(http_req_get);
    }
    else if(req_get_flag == REQUEST_POST_FLAG)
    { 
        struct http_request_post *http_req_post = (struct http_request_post*)http_req_get;
        if (http_req_post->content_type)
        {
            free(http_req_post->content_type);
        }
        if (http_req_post->post_data)
        {
            free(http_req_post->post_data);
        }
        free(http_req_post);
    }

    http_req_get = NULL;
}

void test()
{
	struct event_base* base = event_base_new();
	  
	struct http_request_get *http_req_get = start_http_requset(base,  
															   "http://www.qcwifi.ltd",	
															   REQUEST_GET_FLAG,  
															   NULL, NULL);
	event_base_dispatch(base);
	http_request_free(http_req_get, REQUEST_GET_FLAG);
	event_base_free(base);
}




