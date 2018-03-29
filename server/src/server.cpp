#include "server.hpp"
#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <errno.h>

#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <sys/wait.h>
#include <signal.h>


void usage();
void server_routine(int server_fd);
void get_options(std::string& directory, std::string& port, int argc, char** argv);
void change_dir(std::string directory);
void sigchld_handler(int s);

int main(int argc, char** argv)
{
	struct addrinfo hints, *res;
	int sockfd;
	std::string port = "4513", directory = ".";

	get_options(directory, port, argc, argv);

	std::cout << "port:" << port << "\tdirectory" << directory << std::endl;

	memset(&hints, 0, sizeof hints);
	hints.ai_family = AF_UNSPEC;  // use IPv4 or IPv6, whichever
	hints.ai_socktype = SOCK_STREAM;
	hints.ai_flags = AI_PASSIVE;     // fill in my IP for me

	if (getaddrinfo(NULL, port.c_str(), &hints, &res)) {
		std::cerr << "getaddrinfo" << strerror(errno) << std::endl;
		exit(EXIT_FAILURE);
	}

	sockfd = socket(res->ai_family, res->ai_socktype, res->ai_protocol);

	// bind it to the port we passed in to getaddrinfo():
  if (bind(sockfd, res->ai_addr, res->ai_addrlen) < 0) {
    std::cerr << "bind failed" << strerror(errno) << std::endl;
    exit(EXIT_FAILURE);
  }

  if (listen(sockfd, 5) < 0) { //listen queue is 5
  	std::cerr << "listen failed" << strerror(errno) << std::endl;
  	exit(EXIT_FAILURE);
  }

  std::cout << "meow" << std::endl;
  while (1) {
  	server_routine(sockfd);
  }

  close(sockfd);

  return 0;
}

void server_routine(int server_fd) {
	int new_socket;
	struct sockaddr_storage address; 
	socklen_t addrlen;
	int pid;
	char buffer[SO_SNDBUF];
	struct sigaction sa;

	if ((new_socket = accept(server_fd, (struct sockaddr *)&address, &addrlen)) < 0){
    std::cerr << "accept" << strerror(errno) << std::endl;
    exit(EXIT_FAILURE);
	}

	
	std::cout << "received a request from " << address.__ss_align << std::endl;

  if ((pid = fork()) == 0) {
  	//children
  	int flag;

  	close(server_fd);
  	while((flag = recv(new_socket, buffer, SO_SNDBUF, 0)) != 0) {
  		if (flag == -1) {
  			std::cerr << "child process: invalid recv" << std::endl;
  			exit(EXIT_FAILURE);
  		}

  		std::cout << "recv" << buffer << std::endl;

  		send(new_socket , "msg" , strlen("msg") , 0 );
  	}
  }

	sa.sa_handler = sigchld_handler; // reap all dead processes
	sigemptyset(&sa.sa_mask);
	sa.sa_flags = SA_RESTART;
	if (sigaction(SIGCHLD, &sa, NULL) == -1) {
	    std::cerr << "sigaction" << strerror(errno) << std::endl;
	    exit(EXIT_FAILURE);
	}
}

void get_options(std::string& directory, std::string& port, int argc, char** argv) {
    extern char *optarg;
    int choice, help = 0;

    while ((choice = getopt(argc, argv, "p:d:h")) != EOF) {
      switch (choice) {
        case 'p':
        	port = std::string(optarg);
          break;
        case 'd':
          directory = std::string(optarg);
          break;
        case 'h': 
          help++;
          break;
        case '?': 
          usage();
          exit(-1);
      }
    }
    if (help) {
        usage();
        exit(0);
    }
}


void usage() {
    std::cout<<"server [flags], where flags are:\n"
								"-p # port to serve on (default is 4513)\n"
								"-d dir directory to serve out of (default is /home/clay-pool/dsh)\n"
								"-h this help message\n" << std::endl;
    exit(0);
}

void sigchld_handler(int s) {
    // waitpid() might overwrite errno, so we save and restore it:
    int saved_errno = errno;

    while(waitpid(-1, NULL, WNOHANG) > 0);

    errno = saved_errno;
}