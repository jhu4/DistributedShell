// Zachary Halzel
// Jinan Hu

#include "client.hpp"

//TODO take a look at client.c by beej.us and see if there's good stuff

void print_options(std::string host, std::string command, std::string port) {
  std::cout << "Host server: " << host << std::endl << "Command: " 
  << command << std::endl << "Port: " << port << std::endl;
}

char* get_hashcode(char* rand) {
	return crypt("meow", rand);
}

int main(int argc, char** argv) {
  std::string command, host, port;

  get_options(command, host, port, argc, argv);

  print_options(host, command, port); //TODO remove, just for testing

  struct addrinfo* serv = NULL;  // will point to the results
  int sock = get_sock(host, port, serv);

  // Begin communication
  char buffer[MAX_SIZE] = {0};
  std::cout << "1. Client sends user-name to server (but not password)\n" << std::endl;
  send_to_server(sock, user_name());
  sleep(1);
  std::cout << "2. Server responds by sending back unique random number (using rand() and srand()) \n" << std::endl;
  receive_from_server(sock, buffer, serv);
  std::cout<<"Here is my random number from the server: " << buffer << std::endl;

  std::cout << "Here is the hash code: " << get_hashcode(buffer) << std::endl;
  sleep(1);

  std::cout<<"3. Client encrypts using user’s password plus number as key \n" << std::endl;  
  
  std::string salted_hash = std::string(crypt("meow", buffer));

  std::cout<<"4. Client sends hashed/encrypted value back to server\n" << std::endl;
  send_to_server(sock, salted_hash);
  sleep(1);    
  std::cout<<"5. Server encrypts using the user’s same password plus number as key   \n" << std::endl;
  std::cout<<"6. Server compares two hashed/encrypted values, if same then ok\n" << std::endl;

  send_to_server(sock, command);
  sleep(1);
  receive_from_server(sock, buffer, serv);

  close(sock); 
  freeaddrinfo(serv);
  return 0;
}

int get_sock(std::string host, std::string port, struct addrinfo* serv) {  
  struct addrinfo hints;
  memset(&hints, 0, sizeof(hints)); // make sure the struct is empty
  hints.ai_family = AF_UNSPEC;     // don't care IPv4 or IPv6
  hints.ai_socktype = SOCK_STREAM; // TCP stream sockets

  int status = getaddrinfo(host.c_str(), port.c_str(), &hints, &serv);   
  if (status) {
    perror("getaddrinfo");
    free_exit(serv);
    return -1;
  }               

  int sock = socket(serv->ai_family, serv->ai_socktype, serv->ai_protocol);
  if (sock < 0) {
    perror("Socket creation error");
    free_exit(serv);
    return -1;
  }
  
  if (connect(sock, serv->ai_addr, serv->ai_addrlen) < 0) {
    perror("Connection Failed");
    free_exit(serv);
    return -1;
  }
  return sock;
}

int unique_number(char* buffer, struct addrinfo* serv) {
  int unique_num = atoi(buffer);
  if (unique_num) return unique_num;
  else {
    free_exit(serv);
    return -1;
  }
}

void receive_from_server(int sock, char* buffer, struct addrinfo* serv) {
  memset(buffer, 0, MAX_SIZE);
  int bytes_received = recv(sock, buffer, MAX_SIZE, 0);
  if (!bytes_received) {
    std::cout << "Connection has closed" << std::endl;
    free_exit(serv);
  } else if (bytes_received < 0) {
      perror("recv error");
      free_exit(serv);
  } else 
  std::cout << buffer << std::endl;
}

std::string user_name() {
  char * uname = getlogin();
  std::string uname_string(uname);
  std::cout << "User name:" << uname << std::endl;
  return uname_string;
}

void send_to_server(int sock, std::string msg) {
  unsigned int bytes_sent = send(sock, msg.c_str(), msg.length(), 0);
  if (bytes_sent < 0) perror("send");
  if (bytes_sent < msg.length()) 
      std::cout << "TODO: What if less than everything was sent?" << std::endl;
  else std::cout << "Message sent: " << msg << std::endl;
}

void free_exit(struct addrinfo* serv) {
  freeaddrinfo(serv);
  exit(-1);
}

void get_options(std::string& command, std::string& host, std::string& port, int argc, char** argv) {
  extern int opterr;
  extern char *optarg;
  opterr = 1; /* set to 0 to disable error message */
    
  int choice, help = 0, errflag = 0, port_change = 0;      
  while ((choice = getopt(argc, argv, "c:s:p:h")) != EOF) {
    switch (choice) {
      case 'c':
        command = optarg;
        if (host.length()) errflag = 1;
        break;
      case 's':
        host = optarg;
        if (command.length()) errflag = 1;
        break;
      case 'p': 
        port_change = 1;
        port = optarg;
        break;
      case 'h': 
        help = 1;
        break;
      case '?': 
        usage();
        exit(-1);
    }
  }
  if (help || !errflag) {
    usage();
    exit(0);
  }
  if (!port_change) port = "4513";
}

void usage() {
  std::cout<<"distributed shell client"<<std::endl;
  std::cout<< "usage: dsh [flags] {-c command}, where flags are:"<<std::endl;
  std::cout<<"\t{-c command}    command to execute remotely"<<std::endl;
  std::cout<<"\t{-s host}       host serv is on"<<std::endl;
  std::cout<<"\t[-p #]          port serv is on (default is 4513)"<<std::endl;
  std::cout<<"\t[-h]            this help message"<<std::endl;
  exit(0);
}