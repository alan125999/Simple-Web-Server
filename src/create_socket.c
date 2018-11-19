#include <sys/types.h>
#include <sys/socket.h>

#include "create_socket.h"

int create_socket(int port) {
    int fd_listen, ret;
    struct sockaddr_in addr_server;

    // Create socket
    fd_listen = socket(AF_INET, SOCK_STREAM, 0);
    if (fd_listen == -1){
        perror("Open socket");
        return -1;
    }

    // Set Addr
    bzero(&addr_server, sizeof(addr_server));
    addr_server.sin_family = PF_INET;
    addr_server.sin_addr.s_addr = INADDR_ANY;
    addr_server.sin_port = htons(port);

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
    return fd_listen;
}