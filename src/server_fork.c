#include <stdio.h>
#include <unistd.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <sys/wait.h>

#include "serve_http.h"
#include "listen_socket.h"

#define PORT 8787

void handler_SIGCHLD(int signo)
{
	pid_t	pid;
	int stat;
	
	while ((pid = waitpid(-1, &stat, WNOHANG)) > 0 );
	return;
}



int main(){
    int fd_listen, fd_accept, pid;
    struct sockaddr_in addr_client;
    socklen_t addrlen;

    // Set Workspace
    chdir("../site");

	// Set signal handler to prevent zombie
	signal(SIGCHLD, handler_SIGCHLD);

    // Create Socket
	fd_listen=listen_socket(PORT);
    
	// Accept Client
	addrlen = sizeof(addr_client);
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
			serve_http(fd_accept);
            close(fd_accept);
            return 0;
        } else { 
			// parent
            close(fd_accept);
        }
    }
    return 0;
}


