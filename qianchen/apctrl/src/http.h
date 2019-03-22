#ifndef __HTTP_H__
#define __HTTP_H__

void struct_http_header(char * method, char * path, char * host, char * header_buff);
int get_url_path(char * url, char * host, char * path, int * port);
int get_chunked_size(int socket, char *chunked, int chunkedbufsize, int *chunkedlen);
int get_content_length(char *header, int header_len);

int readChunkedText(int socket, char *buf, int bufsize);
int readStaticText(int socket, int content_length, char *buf);
int readChunkedTextToFile(int socket, char *file);
int readStaticTextToFile(int socket, int content_length, char *file);

int http_add_param(char *param_name, char *param_arg, char *addr);

#endif

