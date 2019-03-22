#ifndef __HTTP_H__
#define __HTTP_H__

#define	MAXLINE	 8192  /* max text line length */
#define MAXBUF   8192  /* max I/O buffer size */

#include "mySocket.h"

void struct_http_header(char * method, char * path, char * host, char * header_buff);
int read_header(int fd, char * buffer, int buffer_len, int *read_end, socketstate_t *state);
int web_html_jump(int client_sock, char * to_url);
int web_302_jump(int client_sock, char * to_url);

int get_url_path(char * url, char * host, char * path, int * port);

int get_http_method(char *header, char *buf, int bufsize);
int get_http_host(char *header, int header_len, char *buf, int bufsize, int *port);
int get_http_uri(char *header, int header_len, char *buf, int bufsize);
int get_chunked_size(int socket, char *chunked, int chunkedbufsize, int *chunkedlen);
int get_content_length(char *header, int header_len);

void change_header_encoding(char * header_buffer, int header_len);
int isHtml(char *header_buff, int header_len, char *uri, int urilen);

char * exist_field_contentlength(char *header, int headerlen);
char * exist_field_chunked(char *header, int headerlen);
char * exist_field_location(char *header, int headerlen);

int readChunkedText(int socket, char *buf, int bufsize);
int readStaticText(int socket, int content_length, char *buf);
int readChunkedTextToFile(int socket, char *file);
int readStaticTextToFile(int socket, int content_length, char *file);

#endif

