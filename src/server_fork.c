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

#include "serve_http.h"

#define BUFSIZE 10240
#define MAX_CONN 100
#define PORT 8787

void handler_SIGCHLD(int signo)
{
	pid_t	pid;
	int stat;
	
	while ((pid = waitpid(-1, &stat, WNOHANG)) > 0 );
	return;
}



int main(){
    int fd_listen, fd_accept, fd_web, ret, pid;
    struct sockaddr_in addr_server, addr_client;
    char buffer[50];
    socklen_t addrlen;

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


