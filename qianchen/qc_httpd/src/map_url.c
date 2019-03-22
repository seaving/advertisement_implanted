#include "includes.h"
#include "map_url_config.h"

int evhttp_map_url_service(struct evhttp_request *req, const t_http_server *http_server, const char *path)
{
	int i, ret = -1;
	
	for (i = 0; ; i ++)
	{
		if (urlMap[i].fun == NULL
		&& urlMap[i].real_path == NULL
		&& urlMap[i].map_name == NULL) {
			break;
		}

		if (urlMap[i].map_name)
		{
			if (strcmp(urlMap[i].map_name, path) == 0)
			{	
				if (urlMap[i].real_path)
				{
					send_html(req, http_server->base_path, urlMap[i].real_path);
					ret = 0;
				}

				if (urlMap[i].fun)
				{
					urlMap[i].fun(req, http_server);
					ret = 0;
				}

				break;
			}
			
			continue;
		}

		if (urlMap[i].real_path)
		{
			if (strcmp(urlMap[i].real_path, path) == 0)
			{				
				if (urlMap[i].fun)
				{
					urlMap[i].fun(req, http_server);
					ret = 0;
				}

				break;
			}

			continue;
		}
	}

	return ret;
}


