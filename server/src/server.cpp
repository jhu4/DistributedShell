#include "server.hpp"

/**
  @author: Zach Halzel, Jinan Hu
**/

static std::unordered_map<std::string, std::string> passwords = { 
  {"zdhalzel", "meow"},
  {"dorothy", "12345"},
  {"username", "password"},
  {"0","1"}
};

static int test = 0;

int main(int argc, char** argv) {
	struct addrinfo hints, *res;
	int sockfd;
	int yes = 1;
	std::string port = PORT;
  char directory[MAX_SIZE];
  
  strcpy(directory, get_current_dir_name());

	srand(time(NULL));

	get_options(directory, port, argc, argv);

	std::cout << "port: " << port << "\tdirectory: " << directory << std::endl;

  if (chdir(directory)) {
    std::cerr << "chdir: " << strerror(errno) << std::endl;
    exit(EXIT_FAILURE);
  }

	memset(&hints, 0, sizeof hints);
	
	set_hints(hints);
	
  do {
  	if (getaddrinfo(NULL, port.c_str(), &hints, &res)) {
  		std::cerr << "getaddrinfo\t" << strerror(errno) << std::endl;
  		exit(EXIT_FAILURE);
  	}

  	if ((sockfd = socket(res->ai_family, res->ai_socktype, res->ai_protocol)) == -1) {
      std::cerr << "socket\t" << strerror(errno) << std::endl;
      continue;
    }

    if (setsockopt(sockfd, SOL_SOCKET, SO_REUSEADDR, &yes, sizeof(int)) == -1) {
      std::cerr << "setsockopt\t" << strerror(errno) << std::endl;
      exit(EXIT_FAILURE);
    }

  	// bind it to the port we passed in to getaddrinfo():
    if (bind(sockfd, res->ai_addr, res->ai_addrlen) < 0) {
      std::cerr << "bind failed\t" << strerror(errno) << std::endl;
      continue;
    }

    break;
  } while (std::cout << "Choose a new port:" << std::endl &&  std::cin >> port);

  if (listen(sockfd, 5) < 0) { //listen queue is 5
    std::cerr << "listen failed" << strerror(errno) << std::endl;
    exit(EXIT_FAILURE);
  }

  std::cout << "Accepting connections" << std::endl;

  while (true) {
  	server_routine(sockfd); 	
  }

  close(sockfd);
  return 0;
}

void set_hints(struct addrinfo &hints) {
  hints.ai_family = AF_UNSPEC;  // use IPv4 or IPv6, whichever
	hints.ai_socktype = SOCK_STREAM;
	hints.ai_flags = AI_PASSIVE;     // fill in my IP for me
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
	
	std::cout << "Connection request received" << std::endl;
	
	pid = fork();
	
	if (pid < 0) {
	  std::cerr << "fork() error " << strerror(errno) << std::endl;
	  exit(EXIT_FAILURE);
	}
  else if (pid == 0) { //children  	
    std::cout << "Forked child" << std::endl;
  	std::stringstream ss;
  	ss << rand();
  	std::string random_num = ss.str();

  	close(server_fd);
    // receive user_name
    receive_from_client(new_socket, buffer);
    std::string username(buffer);
    std::cout << "received username: " << username << std::endl;
    
    // send the random string
	  if (send(new_socket , random_num.c_str(), random_num.length(), 0) == -1) {
	  	std::cerr << "child process: invalid rend" << std::endl;
	  }
	  
	  // receive the hashed password
	  receive_from_client(new_socket, buffer);
    
    std::string hashed_password(buffer);
    if (!authenticate(username, hashed_password, random_num)) {
      char string[] = "Hey, next time use the right password dummy!";
      send(new_socket, string, strlen(string) ,0);
      exit(0);	
    }
    else {
      char string[] = "password ok";
      send(new_socket, string, strlen(string) ,0);
    }

    if (test) exit(0);
    
    //receive the cmd
    receive_from_client(new_socket, buffer);
	  char* cmd = strdup(buffer);
    std::cout << "command: " << cmd << std::endl;

	  close(0);
    close(1);

    if (dup2(new_socket, 1) == -1 || dup2(new_socket, 2) == -1) {
      std::cerr << "dup2:" << strerror(errno) << std::endl;
    }

    pid = fork();

    if (pid < 0) {
      std::cerr << "fork(fork()) error " << strerror(errno) << std::endl;
      exit(EXIT_FAILURE);
      
    }
    else if (pid == 0) {
      execute(cmd);
    }
    else {
      wait(0);
      std::cout << "Cmd executed successfully" << std::endl;
      close(new_socket);
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

void execute(char* cmd) {
  int argv_size = 100;
  char *argv[argv_size];
  memset(argv, 0, argv_size);
  char *token;
  token = strtok(cmd, " ");
  
  int args_len;
  for (args_len = 0; args_len < argv_size && token != NULL; args_len++) {
      argv[args_len] = strdup(token);
      token = strtok(NULL, " ");
  }
  
  argv[args_len] = NULL;

  if (execvp(argv[0], argv)) {
    std::cerr << "execvp:" << strerror(errno) << std::endl;
  }
}

int authenticate(std::string username, std::string hashed_password, std::string random_num) {
  char* rand = (char*) random_num.c_str();
  std::string server_password(get_hashcode(rand, username));

  return !server_password.compare(hashed_password);
}

void receive_from_client(int sock, char* buffer) {
  memset(buffer, 0, MAX_SIZE);
  int bytes_recvd;
  while (!(bytes_recvd = read(sock, buffer, MAX_SIZE))) {
    if (bytes_recvd == -1) {
      close(sock);
      std::cerr << "child process: invalid recv" << std::endl;
      exit(EXIT_FAILURE);
    }
  }
}

char* get_hashcode(char* rand, std::string username) {
	return crypt(passwords[username].c_str(), rand);
}

void get_options(char* directory, std::string& port, int argc, char** argv) {
    extern char *optarg;
    int choice, help = 0;

    while ((choice = getopt(argc, argv, "p:d:th")) != EOF) {
      switch (choice) {
        case 'p':
        	port = std::string(optarg);
          break;
        case 'd':
          strcpy(directory, optarg);
          break;
        case 'h': 
          help++;
          break;
        case 't':
          test = 1;
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
    if (test) std::cout << "Latency Test mode" << std::endl;
}

void usage() {
    std::cout<<"server [flags], where flags are:\n"
								"-p # port to serve on (default is 4513)\n"
								"-d dir directory to serve out of (default is cwd)\n"
								"-t latency test\n"
								"-h this help message\n" << std::endl;
    exit(0);
}

void sigchld_handler(int s) {
    // waitpid() might overwrite errno, so we save and restore it:
    int saved_errno = errno;

    while(waitpid(-1, NULL, WNOHANG) > 0);

    errno = saved_errno;
}