#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/socket.h>

#include "serve_http.h"

#define BUFSIZE 10240


int serve_http(int fd_accept)
{
    int j, file_fd, buflen, len;
    long i, ret;
    char * type;
    char buffer[BUFSIZE+1];
	
	// Read Request Header
	ret = read(fd_accept, buffer, BUFSIZE);
	if(ret <= 0) {
		perror("Read Reqest");
		return -1;
	}
	buffer[ret] = '\0';
	
	// Give up if not GET method
	if (strncmp(buffer,"GET ",4) && strncmp(buffer,"get ",4)){
		send(fd_accept, header_405, strlen(header_405), 0);
		return -1;
	}
	
	// cut string, only GET xxx remaining
    for(i=4;i<BUFSIZE;i++) {
        if(buffer[i] == ' ') {
            buffer[i] = '\0';
            break;
        }
    }
	
	// refuse .. in path for security 
    for (j=0; j<i-1; j++)
        if (buffer[j]=='.' && buffer[j+1]=='.'){
            send(fd_accept, header_403, strlen(header_403), 0);
			return -1;
		}
	
	// if path == /, use index.html 
	buflen = strlen(buffer);
    if (buffer[buflen - 1] == '/')
        strcpy(&buffer[buflen], "index.html");
	
	puts(buffer);
	
	// Detect extension
    buflen = strlen(buffer);
    type = NULL;
    for(i=0; extensions[i].ext!=0; i++) {
        len = strlen(extensions[i].ext);
        if(!strncmp(&buffer[buflen-len], extensions[i].ext, len)) {
            type = extensions[i].filetype;
            break;
        }
    }
	
	// Unsupported extension
    if(type == 0) {
        type = extensions[i-1].filetype;
    }
	
	// Open File
	if((file_fd=open(&buffer[5],O_RDONLY))==-1) {
		send(fd_accept, header_404, strlen(header_404), 0);
		return 0;
	}
	
	// Send Response HTTP Header
	sprintf(buffer, "HTTP/1.0 200 OK\r\nContent-Type: %s\r\n\r\n", type);
	send(fd_accept, buffer, strlen(buffer), 0);
	
	while((ret = read(file_fd, buffer, sizeof(buffer))) > 0){
		send(fd_accept, buffer, ret, 0);
	}
	close(file_fd);
	close(fd_accept);
	return 0;
}