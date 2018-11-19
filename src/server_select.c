#include <stdio.h>
#include <unistd.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <sys/select.h>

#include "serve_http.h"
#include "listen_socket.h"

#define PORT 8787

int main(){
    int fd_listen, fd_accept, ret, fd_max ,i;
    struct sockaddr_in addr_client;
	socklen_t addrlen;
	fd_set readfds, allfds;

	// Set Workspace
    if(chdir("../site") == -1) {
        perror("chdir");
        return -1;
    }

    // Create socket
    fd_listen=listen_socket(PORT);
    if(fd_listen < 0) return -1;
	
	// Init fd set
	FD_ZERO(&allfds);
	FD_SET(fd_listen, &allfds);
	fd_max = fd_listen;
	
	// Accept Client
	addrlen = sizeof(addr_client);
    while(1){
		// Copy fd set
		readfds = allfds;
		
		// Select
		ret = select(fd_max + 1, &readfds, NULL, NULL, NULL);
		if (ret == -1) {
            perror("select");
            return -1;
        } else if (ret == 0) {
            printf("select timeout\n");
            continue;
        } else {
			for (i = 0; i < FD_SETSIZE; i++) {
				if (FD_ISSET(i, &readfds)) {
					if(i == fd_listen) {
						fd_accept = accept(fd_listen, (struct sockaddr *) &addr_client, &addrlen);
						if (fd_accept == -1) continue;
						FD_SET(fd_accept, &allfds);
						if (fd_accept > fd_max)
							fd_max = fd_accept;
					} else {
						serve_http(i);
						close(i);
						FD_CLR(i, &allfds);
					}
				}
			}
		}
	}

    return 0;
}


