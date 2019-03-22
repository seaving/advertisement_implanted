#include "includes.h"
#include "config_filter.h"

bool _filter_list_match(filter_list *conf, char *path)
{
	int i = 0;
	for (i = 0; conf[i].path; i ++)
	{
		if (straddr(path, conf[i].path) == path)
			return true;
	}

	return false;
}

char *evhttp_get_path(struct evhttp_request *req)
{
	struct evhttp_uri *decoded = NULL;

	const char *uri = NULL;
	const char *path = NULL;
	char *decoded_path = NULL;

	uri = evhttp_request_get_uri(req);
	if (! uri)
		goto err;
	
	/* Decode the URI */
	decoded = evhttp_uri_parse(uri);
	if (! decoded)
	{
		goto err;
	}

	//LOG_NORMAL_INFO("uri: %s\n", evhttp_uri_get_query(decoded));

	/* Let's see what path the user asked for. */
	path = evhttp_uri_get_path(decoded);
	if (! path) path = "/";

	/* We need to decode it, to see what path the user really wanted. */
	decoded_path = evhttp_uridecode(path, 0, NULL);
	if (decoded_path == NULL)
		goto err;		

	evhttp_uri_free(decoded);
	return decoded_path;

err:
	if (decoded)
		evhttp_uri_free(decoded);
	if (decoded_path)
		free(decoded_path);

	return NULL;
}

char *evhttpd_request_find_head_info(struct evkeyvalq *header, const char *key)
{
	if (! header) return NULL;
	
    struct evkeyval *first_node = header->tqh_first;
    if (! first_node) LOG_ERROR_INFO("header evkeyvalq null!\n");
    
    while (first_node)
    {
        //LOG_NORMAL_INFO("%s: %s\r\n", first_node->key, first_node->value);
        if (strcasecmp(first_node->key, key) == 0)
        {
			return first_node->value;
        }
        
        first_node = first_node->next.tqe_next;
    }

    return NULL;
}

session_t evhttpd_filter_session(struct evhttp_request *req)
{
	char *path = evhttp_get_path(req);
	if (! path) return E_SESSION_INVALID;
	
	if (_filter_list_match(session_wlist, path) == true)
	{
		free(path);
		return E_SESSION_OK;
	}
	free(path);
	
	char *cookie = evhttpd_request_find_head_info(req->input_headers, "Cookie");
	if (! cookie)
		return E_SESSION_INVALID;
	
	LOG_HL_INFO("Cookie: %s\n", cookie);

	if (cookie_check(cookie) == true)
		return E_SESSION_OK;

	return E_SESSION_INVALID;
}

passwd_t evhttpd_filter_password(struct evhttp_request *req)
{
	char *path = evhttp_get_path(req);
	if (! path)
		return E_PASSWD_INVALID;

	if (_filter_list_match(passwd_list, path) == false)
	{
		free(path);
		return E_NOT_PASSWD_LOGIN;
	}
	free(path);
	
	char *passwd = evhttp_get_post_parm(req, "password");
	if (! passwd)
		return E_PASSWD_INVALID;
	
	if (! passwd)
		return E_PASSWD_INVALID;
	
	LOG_HL_INFO("password: %s\n", passwd);
	
	if (passwd_check(passwd) == true)
	{
		free(passwd);
		return E_PASSWD_OK;
	}

	free(passwd);
	return E_PASSWD_INVALID;
}

