#include "includes.h"

static const struct table_entry {
	const char *extension;
	const char *content_type;
} content_type_table[] = {
	{ "html", "text/html" },
	{ "htm", "text/htm" },
	{ "css", "text/css" },
	{ "js", "application/javascript"},
	{ "gif", "image/gif" },
	{ "jpg", "image/jpeg" },
	{ "jpeg", "image/jpeg" },
	{ "png", "image/png" },
	{ NULL, NULL },
};

static const char *get_content_extension(const char *path)
{
	const char *last_period, *extension;
	last_period = strrchr(path, '.');
	if (!last_period || strchr(last_period, '/'))
		return NULL; /* no exension */
	extension = last_period + 1;
	return extension;
}

static const char *guess_content_type(const char *extension)
{
	const struct table_entry *ent;
	if (extension == NULL)
		goto not_found;

	for (ent = &content_type_table[0]; ent->extension; ++ent)
	{
		if (! evutil_ascii_strcasecmp(ent->extension, extension))
			return ent->content_type;
	}

not_found:
	return "text/html";
}

static void webserver_cb(struct evhttp_request *req, void *arg)
{
	struct evbuffer *evb = NULL;
	const char *docroot = arg;
	const char *uri = evhttp_request_get_uri(req);
	struct evhttp_uri *decoded = NULL;
	const char *path;
	char *decoded_path;
	char *whole_path = NULL;
	size_t len;
	int fd = -1;
	struct stat st;

	if (evhttp_request_get_command(req) != EVHTTP_REQ_GET)
	{
		// only support HTTP GET
		return;
	}

	/* Decode the URI */
	decoded = evhttp_uri_parse(uri);
	if (! decoded)
	{
		evhttp_send_error(req, HTTP_BADREQUEST, 0);
		return;
	}

	LOG_WARN_INFO("uri: %s\n", uri);

	/* Let's see what path the user asked for. */
	path = evhttp_uri_get_path(decoded);
	if (! path) path = "/";

	/* We need to decode it, to see what path the user really wanted. */
	decoded_path = evhttp_uridecode(path, 0, NULL);
	if (decoded_path == NULL)
		goto err;
		
	LOG_WARN_INFO("path: %s\n", path);

	const char *extension = get_content_extension(decoded_path);
	if (extension == NULL || strncmp(extension, "htm", 3) == 0)
	{
		len = strlen("403.html") + strlen(docroot) + 2;
		if (! (whole_path = malloc(len)))
		{
			LOG_PERROR_INFO("malloc error.");
			goto err;
		}
		evutil_snprintf(whole_path, len, "%s/403.html", docroot);
	}
	else
	{
		len = strlen(decoded_path) + strlen(docroot) + 2;
		if (! (whole_path = malloc(len)))
		{
			LOG_PERROR_INFO("malloc error.");
			goto err;
		}
		#if 0
		if (decoded_path[0] == '/')
		{
			evutil_snprintf(whole_path, len, "%s%s", docroot, decoded_path);
		}
		else
		{
			evutil_snprintf(whole_path, len, "%s/%s", docroot, decoded_path);
		}
		#endif
	}

	LOG_NORMAL_INFO("whole_path: %s\n", whole_path);
	if (stat(whole_path, &st) < 0)
	{
		LOG_PERROR_INFO("stat error.");
		goto err;
	}

	evb = evbuffer_new();

	if (S_ISREG(st.st_mode))
	{
		/* Otherwise it's a file; add it to the buffer to get
		 * sent via sendfile */
		
		const char *type = guess_content_type(extension);
		if ((fd = open(whole_path, O_RDONLY)) < 0)
		{
			LOG_PERROR_INFO("open %s error.", whole_path);
			goto err;
		}

		if (fstat(fd, &st) < 0)
		{
			/* Make sure the length still matches, now that we
			 * opened the file :/ */
			LOG_PERROR_INFO("fstat error.");
			goto err;
		}

		evhttp_add_header(evhttp_request_get_output_headers(req),
		    "Content-Type", type);
		evbuffer_add_file(evb, fd, 0, st.st_size);
	}
	
	evhttp_send_reply(req, 200, "OK", evb);
	goto done;

err:
	evhttp_send_error(req, 404, "Document was not found");
	if (fd >= 0)
		close(fd);
done:
	if (decoded)
		evhttp_uri_free(decoded);
	if (decoded_path)
		free(decoded_path);
	if (whole_path)
		free(whole_path);
	if (evb)
		evbuffer_free(evb);
}

static int web_server(const char *address, const t_http_server *http_server)
{
	struct event_base *base;
	struct evhttp *http;
	struct evhttp_bound_socket *handle;

	base = event_base_new();
	if (! base)
	{
		LOG_PERROR_INFO("event_base_new failt !\n");
		goto end_loop;
	}

	/* Create a new evhttp object to handle requests. */
	http = evhttp_new(base);
	if (! http)
	{
		goto end_loop;
	}

	evhttp_set_gencb(http, webserver_cb, (void *)http_server->base_path);

	handle = evhttp_bind_socket_with_handle(http, address, http_server->gw_http_port);
	if (!handle)
	{
		goto end_loop;
	}

	event_base_dispatch(base);

end_loop:
	if (handle)
		evhttp_del_accept_socket(http, handle);

	if (http)
		evhttp_free(http);

	if (base)
		event_base_free(base);

	return -1;
}

//zhe ge shi  web server
void *_create_web_server_(void *arg)
{
	pthread_detach(pthread_self());

	t_http_server http_server;

	for ( ; ; )
	{
		http_server.gw_http_port = QC_DOG_WEB_SERVER_PORT;
		http_server.base_path = "/etc/app/qcdog";

		char lan_ip[33] = {0};
		get_dev_ip(lan_ip, GET_LAN_NAME);
		web_server(lan_ip, &http_server);

		sleep(1);
	}

	return NULL;
}


