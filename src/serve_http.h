#ifndef SERVER_HTTP_H
#define SERVER_HTTP_H

#define BUFSIZE 10240

const char header_405[] = "HTTP/1.1 405 Method Not Allowed \r\n\r\n";
const char header_404[] = "HTTP/1.1 404 Not Found \r\n\r\n";
const char header_403[] = "HTTP/1.1 403 Forbidden \r\n\r\n";

const struct {
    char *ext;
    char *filetype;
} extensions [] = {
    {"gif", "image/gif" },
    {"jpg", "image/jpeg"},
    {"jpeg","image/jpeg"},
    {"png", "image/png" },
    {"zip", "image/zip" },
    {"gz",  "image/gz"  },
    {"tar", "image/tar" },
    {"htm", "text/html" },
    {"html","text/html" },
    {"exe","text/plain" },
    {0,0} };
	
int serve_http(int fd_accept);

#endif