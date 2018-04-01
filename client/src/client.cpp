/**
  @author: Zach Halzel, Jinan Hu
**/

#include "client.hpp"

static std::unordered_map<std::string, std::string> passwords = { 
  {"zdhalzel", "meow"},
  {"dorothy", "12345"}
};

int main(int argc, char** argv) {
  std::string command, host, port;

  get_options(command, host, port, argc, argv);
  struct addrinfo* serv = NULL; 
  int sock = get_sock(host, port, serv);

  // Begin communication
  char buffer[MAX_SIZE] = {0};
  //Client sends user-name to server
  send_to_server(sock, user_name());

  // Server responds by sending back unique random number
  receive_from_server(sock, buffer, serv);
  

  //Client encrypts using user’s password plus number as key
  
  std::string salted_hash = get_hashcode(buffer);

  // Client sends hashed/encrypted value back to server
  send_to_server(sock, salted_hash);
  
  //recieve the result of hashed code
  receive_from_server(sock, buffer, serv);
  std::cout << buffer << std::endl;

  send_to_server(sock, command);

  receive_from_server(sock, buffer, serv);
  std::cout << buffer << std::endl;

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

char* get_hashcode(char* rand) {
  std::string username(getlogin());
	return crypt(passwords[username].c_str(), rand);
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
  int bytes_received;
  while (!(bytes_received = read(sock, buffer, MAX_SIZE))) {
    if (bytes_received == -1) {
      std::cerr << "recv error: " << strerror(errno) << std::endl;
      free_exit(serv);
    } 
  }
}

std::string user_name() {
  char* uname = getlogin();
  std::string uname_string(uname);
  std::cout << "User name:" << uname << std::endl;
  return uname_string;
}

void send_to_server(int sock, std::string msg) {
  int length = msg.length();
  int bytes_sent = send_all(sock, (char*) msg.c_str(), &length);
  if (bytes_sent) perror("send");
}

int send_all(int sock, char* buf, int* len) {
  //http://beej.us/guide/bgnet/html/single/bgnet.html
    int total = 0;        // how many bytes we've sent
    int bytesleft = *len; // how many we have left to send
    int n;
    while(total < *len) {
        n = send(sock, buf+total, bytesleft, 0);
        if (n == -1) { break; }
        total += n;
        bytesleft -= n;
    }
    *len = total; // return number actually sent here
    return n==-1?-1:0; // return -1 on failure, 0 on success
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