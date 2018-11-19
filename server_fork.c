#include <stdio.h>
#include <stdlib.h>
#include <netinet/in.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/stat.h>
#include <sys/wait.h>

#define BUFSIZE 10240
#define MAX_CONN 100
#define PORT 8787

char header_405[] = "HTTP/1.1 405 Method Not Allowed \r\n\r\n";
char header_404[] = "HTTP/1.1 404 Not Found \r\n\r\n";
char header_403[] = "HTTP/1.1 403 Forbidden \r\n\r\n";

struct {
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
	
void handler_SIGCHLD(int signo)
{
	pid_t	pid;
	int stat;
	
	while ((pid = waitpid(-1, &stat, WNOHANG)) > 0 );
	return;
}

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

int main(){
    int fd_listen, fd_accept, addrlen, fd_web, ret, pid;
    struct sockaddr_in addr_server, addr_client;
    char buffer[50];

	// Set signal handler to prevent zombie
	signal(SIGCHLD, handler_SIGCHLD);
	
    // Create socket
    fd_listen = socket(AF_INET, SOCK_STREAM, 0);
    if (fd_listen == -1){
        perror("Open socket");
        return -1;
    }

    // Set Addr
    addrlen = sizeof(addr_client);
    bzero(&addr_server, sizeof(addr_server));
    addr_server.sin_family = PF_INET;
    addr_server.sin_addr.s_addr = INADDR_ANY;
    addr_server.sin_port = htons(PORT);

    // bind
    ret = bind(fd_listen, (struct sockaddr *) &addr_server, sizeof(addr_server));
	if(ret < 0){
		perror("bind");
		return -1;
	}
    listen(fd_listen, MAX_CONN);
	if(ret < 0){
		perror("listen");
		return -1;
	}
	
	// Accept Client
    while(1){
        fd_accept = accept(fd_listen, (struct sockaddr *) &addr_client, &addrlen);
		if (fd_accept == -1) continue;
		
		// Fork
        if ((pid = fork()) < 0) {
			perror("fork");
            return -1;
        } else if (pid == 0) {  
			// child
            close(fd_listen);
			return serve_http(fd_accept);
        } else { 
			// parent
            close(fd_accept);
        }
		
        
    }

    return 0;
}


