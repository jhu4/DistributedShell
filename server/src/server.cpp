#include "server.hpp"
#include <stdio.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <unistd.h>
#include <stdlib.h>
#include <netinet/in.h>
#include <string.h>
#define PORT 8080


int main(int argc, char const *argv[])
{
    int server_fd, new_socket, pid, port;
    struct sockaddr_in address;
    int addrlen = sizeof(address);
    char buffer[SO_SNDBUF] = {0};
      
    // Creating socket file descriptor
    if ((server_fd = socket(AF_INET, SOCK_STREAM, 0)) == 0)
    {
        perror("socket failed");
        exit(EXIT_FAILURE);
    }
  
  	bzero((char *) &address, sizeof(address));
    address.sin_family = AF_INET;
    address.sin_addr.s_addr = htonl(INADDR_ANY);
    address.sin_port = htons( PORT );
      
    // Forcefully attaching socket to the port 8080
    if (bind(server_fd, (struct sockaddr *) &address, sizeof(address)) < 0) {
        perror("bind failed");
        exit(EXIT_FAILURE);
    }

    while (listen(server_fd, 3) == 0) {
    	if ((new_socket = accept(server_fd, (struct sockaddr *)&address, 
                       (socklen_t*)&addrlen))<0){
        perror("accept");
        exit(EXIT_FAILURE);
    	}

	    pid = fork();
	    if (pid == 0) {
	    	//children
	    	int flag;
	    	while((flag = recv(new_socket, buffer, SO_SNDBUF, 0)) != 0) {
	    		if (flag == -1) {
	    			std::cerr << "child process: invalid recv" << std::endl;
	    			return -1;
	    		}

	    		std::cout << "recv" << buffer << std::endl;

	    		send(new_socket , "msg" , strlen("msg") , 0 );
	    	}


	    }

    }

    return 0;
}