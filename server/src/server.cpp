#include "server.hpp"
#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <unistd.h>

#include <iostream>
#include <string>
#include <sstream>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <sys/wait.h>
#include <signal.h>
#include <crypt.h>

//TODO add hashtable of users to passwords

void usage();
void server_routine(int server_fd);
void get_options(std::string& directory, std::string& port, int argc, char** argv);
void change_dir(std::string directory);
void sigchld_handler(int s);
char* get_hashcode(char* rand);

int main(int argc, char** argv)
{
	struct addrinfo hints, *res;
	int sockfd;
	int yes = 1;
	std::string port = "4513", directory = ".";
	srand(time(NULL));

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

  if (setsockopt(sockfd, SOL_SOCKET, SO_REUSEADDR, &yes, sizeof(int)) == -1) {
      std::cerr << "setsockopt" << strerror(errno) << std::endl;
      exit(EXIT_FAILURE);
  }

	// bind it to the port we passed in to getaddrinfo():
  if (bind(sockfd, res->ai_addr, res->ai_addrlen) < 0) {
    std::cerr << "bind failed" << strerror(errno) << std::endl;
    exit(EXIT_FAILURE);
  }

  if (listen(sockfd, 5) < 0) { //listen queue is 5
  	std::cerr << "listen failed" << strerror(errno) << std::endl;
  	exit(EXIT_FAILURE);
  }

  while (true) {
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
    std::cerr << "accept() " << strerror(errno) << std::endl;
    exit(EXIT_FAILURE);
	}

	
	std::cout << "received a request from " << address.__ss_align << std::endl;
  if ((pid = fork()) == 0) {
  	//children
  	int flag;
  	std::stringstream ss;
  	ss << rand();
  	std::string random_num = ss.str();

  	close(server_fd);
    // receive user_name
	  while (!(flag = recv(new_socket, buffer, SO_SNDBUF, 0))) {
	  	if (flag == -1) {
	  		close(new_socket);
				std::cerr << "child process: invalid recv" << std::endl;
				exit(EXIT_FAILURE);
	  	}
	  }
	  std::cout << "recv 1st " << buffer << std::endl;
    
    sleep(1);
    // send the random string
	  if (send(new_socket , random_num.c_str(), random_num.length(), 0) == -1) {
	  	std::cerr << "child process: invalid rend" << std::endl;
	  }
	  std::cout << "Here is the number I sent:" << random_num << std::endl;
	  
	  
	  sleep(1);
	  // receive the hashed password
	  while (!(flag = recv(new_socket, buffer, SO_SNDBUF, 0))) {
	  	if (flag == -1) {
	  		close(new_socket);
				std::cerr << "child process: invalid recv" << std::endl;
				exit(EXIT_FAILURE);
	  	}
	  }

	  std::cout << "recv 2nd " << buffer << std::endl;
	  sleep(1);
  // TODO send the result of the terminal
	  if (send(new_socket , "msg", strlen("msg") , 0 ) == -1) {
	  	std::cerr << "child process: invalid rend" << std::endl;
	  }
		  

	  exit(0);
  }

	sa.sa_handler = sigchld_handler; // reap all dead processes
	sigemptyset(&sa.sa_mask);
	sa.sa_flags = SA_RESTART;
	if (sigaction(SIGCHLD, &sa, NULL) == -1) {
	    std::cerr << "sigaction" << strerror(errno) << std::endl;
	    exit(EXIT_FAILURE);
	}
	close(new_socket);
}

char* get_hashcode(char* rand) {
	return crypt("meow", rand);
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