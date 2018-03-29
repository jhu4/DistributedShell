#include "client.hpp"

int main(int argc, char** argv) {
    // struct sockaddr_in address;
    std::string command, host, port;

    get_options(command, host, port, argc, argv);
    
    //TODO remove, just for testing
    std::cout << "Host server: " << host << std::endl
                << "Command: " << command << std::endl
                << "Port: " << port << std::endl;
                
    /* http://beej.us/guide/bgnet/html/single/bgnet.html#getaddrinfo */
    struct addrinfo hints;
    struct addrinfo* serv;  // will point to the results
    memset(&hints, 0, sizeof(hints)); // make sure the struct is empty
    hints.ai_family = AF_UNSPEC;     // don't care IPv4 or IPv6
    hints.ai_socktype = SOCK_STREAM; // TCP stream sockets

    // get ready to connect
    int status = getaddrinfo(host.c_str(), port.c_str(), &hints, &serv);
    
    if(status) {
        perror("getaddrinfo");
        free_exit(serv);
    }               

    char buffer[MAX_SIZE] = {0};
    int sock = socket(serv->ai_family, serv->ai_socktype, serv->ai_protocol);
    if (sock < 0) {
        perror("Socket creation error");
        free_exit(serv);
    }
  
    if (connect(sock, serv->ai_addr, serv->ai_addrlen) < 0) {
        perror("Connection Failed");
        free_exit(serv);
    }
    
    int bytes_sent = send(sock, command.c_str(), command.length(), 0);
    if (bytes_sent < 0) perror("send");

    if (bytes_sent < command.length()) std::cout << "TODO" << std::endl;
    
    else {
        std::cout << "Command message sent" << std::endl;
        int bytes_received = recv(sock, buffer, MAX_SIZE, 0);
        if (bytes_received < 0) perror("recv error");
        std::cout << buffer << std::endl;
    }
    
    freeaddrinfo(serv);
    return 0;
}

void free_exit(struct addrinfo* serv) {
    freeaddrinfo(serv);
    exit(-1);
}

void get_options(std::string& command, std::string& host, std::string& port, int argc, char** argv) {
    extern int optind, opterr;
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