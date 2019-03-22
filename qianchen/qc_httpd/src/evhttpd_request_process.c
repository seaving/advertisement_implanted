#include "includes.h"


static const struct table_entry {
	const char *extension;
	const char *content_type;
} content_type_table[] = {
	{ "txt", 	"text/plain" },
	{ "c", 		"text/plain" },
	{ "h", 		"text/plain" },
	{ "html", 	"text/html" },
	{ "htm", 	"text/html" },
	{ "shtml", 	"text/html" },
	{ "xhtml", 	"text/html" },
	{ "dhtml", 	"text/html" },
	{ "jsp", 	"text/html" },
	{ "asp", 	"text/html" },
	{ "php", 	"text/html" },
	{ "css", 	"text/css" },
	{ "js", 	"application/javascript"},
	{ "gif", 	"image/gif" },
	{ "jpg", 	"image/jpeg" },
	{ "jpeg", 	"image/jpeg" },
	{ "png", 	"image/png" },
	{ "svg", 	"image/svg+xml" },
	{ "pdf", 	"application/pdf" },
	{ "ps", 	"application/postscript" },
	{ NULL, 	NULL },
};

const char *get_content_extension(const char *path)
{
	const char *last_period = NULL, *extension = NULL;
	
	last_period = strrchr(path, '.');
	if (! last_period || strchr(last_period, '/'))
		return NULL; /* no exension */
	
	extension = last_period + 1;
	return extension;
}

/* Try to guess a good content-type for 'path' */
const char *guess_content_type(const char *extension)
{
	const struct table_entry *ent;
	if (extension == NULL)
		goto not_found;

	for (ent = &content_type_table[0]; ent->extension; ++ ent)
	{
		if (! evutil_ascii_strcasecmp(ent->extension, extension))
			return ent->content_type;
	}

not_found:
	return "text/html";
}

int send_html(struct evhttp_request *req, const char *docroot, const char *path)
{
	struct evbuffer *evb = NULL;
	
	const char *extension = NULL;
	char *whole_path = NULL;
	size_t len = 0;
	
	struct stat st;

	int fd = -1;

	len = strlen(docroot) + strlen(path) + 2;
	whole_path = calloc(1, len);
	if (! whole_path)
		goto err;
	
	evutil_snprintf(whole_path, len, "%s%s", docroot, path);
	LOG_NORMAL_INFO("whole_path: %s\n", whole_path);
	
	if (stat(whole_path, &st) < 0)
	{
		goto err;
	}

	evb = evbuffer_new();
	if (! evb)
		goto err;

	if (S_ISREG(st.st_mode))
	{
		/* Otherwise it's a file; add it to the buffer to get
		 * sent via sendfile */
		extension = get_content_extension(path);
		const char *type = guess_content_type(extension);
		if ((fd = open(whole_path, O_RDONLY)) < 0)
			goto err;

		if (fstat(fd, &st) < 0)
		{
			/* Make sure the length still matches, now that we
			 * opened the file :/ */
			goto err;
		}

		evhttp_add_header(evhttp_request_get_output_headers(req),
			"Content-Type", type);
		//evhttp_add_header(evhttp_request_get_output_headers(req),
			//"Content-Encoding", "gzip");
		LOG_NORMAL_INFO("st.st_size: %lld\n", st.st_size);
		evbuffer_add_file(evb, fd, 0, st.st_size);
	}
	else
	{
		goto err;
	}
	
	evhttp_send_reply(req, 200, "OK", evb);
	goto done;
	
err:
	evhttp_send_error(req, 404, "Document was not found");
	if (fd > 0)
		close(fd);
	free_malloc(whole_path);
	if (evb)
		evbuffer_free(evb);
	return -1;

done:
	free_malloc(whole_path);
	if (evb)
		evbuffer_free(evb);

	return 0;

}

int _evhttpd_parse_request(struct evhttp_request *req, char **decoded_path)
{
	struct evhttp_uri *decoded = NULL;
	
	const char *path = NULL;

	const char *uri = evhttp_request_get_uri(req);
	if (! uri)
		goto err;
	
	/* Decode the URI */
	decoded = evhttp_uri_parse(uri);
	if (! decoded)
	{
		goto err;
	}

	LOG_NORMAL_INFO("uri: %s\n", evhttp_uri_get_query(decoded));

	/* Let's see what path the user asked for. */
	path = evhttp_uri_get_path(decoded);
	if (! path) path = "/";

	/* We need to decode it, to see what path the user really wanted. */
	*decoded_path = evhttp_uridecode(path, 0, NULL);
	if (*decoded_path == NULL)
		goto err;
	
	LOG_NORMAL_INFO("path: %s\n", *decoded_path);
	
	if (decoded)
		evhttp_uri_free(decoded);
	return 0;

err:
	if (decoded)
		evhttp_uri_free(decoded);
	return -1;
}

int _evhttpd_map_url_call(struct evhttp_request *req, const t_http_server *http_server, const char *path)
{
	return evhttp_map_url_service(req, http_server, path);
}

int evhttpd_get(struct evhttp_request *req, const t_http_server *http_server)
{
	const char *decoded_path = NULL;

	if (_evhttpd_parse_request(req, (char **)(&decoded_path)) < 0)
		goto err;

	if (! decoded_path) goto err;

	//处理cgi和url映射
	if (_evhttpd_map_url_call(req, http_server, decoded_path) >= 0)
		goto done;

	//没有被_evhttpd_map_url_call处理的请求走这里
	if (send_html(req, http_server->base_path, decoded_path) >= 0)
		goto done;
	
err:
	evhttp_send_error(req, 404, "Document was not found");

done:
	if (decoded_path)
		free((char *)decoded_path);

	return 0;
}

int evhttpd_post(struct evhttp_request *req, const t_http_server *http_server)
{
	return evhttpd_get(req, http_server);
}


