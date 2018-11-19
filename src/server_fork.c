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
#include "create_socket.h"

#define PORT 8787

void handler_SIGCHLD(int signo)
{
	pid_t	pid;
	int stat;
	
	while ((pid = waitpid(-1, &stat, WNOHANG)) > 0 );
	return;
}



int main(){
    int fd_listen, fd_accept, ret, pid;
    struct sockaddr_in addr_server, addr_client;
    socklen_t addrlen;

	// Set signal handler to prevent zombie
	signal(SIGCHLD, handler_SIGCHLD);
	fd_listen=create_socket(PORT);
    
	addrlen = sizeof(addr_client);
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


