#include "includes.h"

void evhttp_gw_reply_302_redirect(struct evhttp_request *req, const char *redirect_url)
{
	struct evbuffer *evb = evbuffer_new();
	
	if (! evb)
		goto err;

	evhttp_add_header(evhttp_request_get_output_headers(req),
				"Location", redirect_url);
	evhttp_send_reply(req, 302, "Found", evb);

err:
	if (evb)
		evbuffer_free(evb);
}

void evhttp_gw_reply_302_redirect_with_cookie(struct evhttp_request *req, const char *redirect_url, const char *cookie)
{
	struct evbuffer *evb = evbuffer_new();
	
	if (! evb)
		goto err;

	evhttp_add_header(evhttp_request_get_output_headers(req),
				"Location", redirect_url);
	evhttp_add_header(evhttp_request_get_output_headers(req),
				"Set-Cookie", cookie);
	evhttp_send_reply(req, 302, "Found", evb);
	
err:
	if (evb)
		evbuffer_free(evb);
}

char *evhttp_get_parm(struct evhttp_request *req, const char *key)
{
	const char *uri = evhttp_request_get_uri(req);
	if (! uri) return NULL;
	
	char *decoded_uri = evhttp_decode_uri(uri);
	if (! decoded_uri) return NULL;

	struct evkeyvalq params;
	evhttp_parse_query(decoded_uri, &params);
	free(decoded_uri);

	const char *value = evhttp_find_header(&params, key);
	if (! value) return NULL;

	return evhttp_decode_uri(value);
}

char *evhttp_get_post_parm(struct evhttp_request *req, const char *key)
{
	struct evkeyvalq params;
	struct evbuffer *buf = evhttp_request_get_input_buffer(req);
	
	size_t len = evbuffer_get_length(buf);
	if (len > 1024)
		return NULL;
	
	char *post_data = calloc(1, len + 5);
	if (! post_data) return NULL;

	memcpy(post_data, "/?", 2);

	/*
	size_t offset = 0;
	while (len > 0)
	{
		int n;
		char cbuf[128];
		n = evbuffer_copyout(buf, cbuf, sizeof(cbuf));
		if (n <= 0)
			break;
		memcpy(post_data + offset, cbuf, n);
		offset += n;
		len -= n;
	}*/

	evbuffer_copyout(buf, post_data + strlen(post_data), len);
	LOG_HL_INFO("post data: %s\n", post_data);
	
	evhttp_parse_query(post_data, &params);
	
	const char *value = evhttp_find_header(&params, key);
	if (! value)
	{
		free_malloc(post_data);
		return NULL;
	}
	free_malloc(post_data);
		
	return evhttp_decode_uri(value);
}

char *evhttpd_parse_parma_by_uri(char *uri, char *key)
{
	struct evkeyvalq params;
	evhttp_parse_query(uri, &params);
	
	const char *value = evhttp_find_header(&params, key);
	if (! value)
	{
		return NULL;
	}
		
	return evhttp_decode_uri(value);
}

int evhttpd_send_200_response(struct evhttp_request *req, char *type, const char *fmt, ...)
{
	struct evbuffer *evb = evbuffer_new();
	if (! evb) return -1;

	va_list ap;
	va_start(ap, fmt);
	evbuffer_add_vprintf(evb, fmt, ap);
	va_end(ap);

	if (type)
	{
		evhttp_add_header(evhttp_request_get_output_headers(req),
			    "Content-Type", type);
	}
	evhttp_add_header(evhttp_request_get_output_headers(req),
	    	"Connection", "closed");
	evhttp_send_reply (req, 200, "OK", evb);

	if (evb) evbuffer_free(evb);
	return 0;
}



