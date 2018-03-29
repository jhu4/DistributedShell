#include "client.hpp"

int main(int argc, char** argv) {
    // struct sockaddr_in address;
    std::string command, host, port;

    get_options(command, host, port, argc, argv);
    
    //TODO remove
    std::cout << "Host server: " << host << std::endl
                << "Command: " << command << std::endl
                << "Port: " << port << std::endl; 
                
                
    /* http://beej.us/guide/bgnet/html/single/bgnet.html#getaddrinfo */
    int status;
    struct addrinfo hints;
    struct addrinfo *servinfo;  // will point to the results
    memset(&hints, 0, sizeof(hints)); // make sure the struct is empty
    hints.ai_family = AF_UNSPEC;     // don't care IPv4 or IPv6
    hints.ai_socktype = SOCK_STREAM; // TCP stream sockets

    // get ready to connect
    status = getaddrinfo(host.c_str(), port.c_str(), &hints, &servinfo);
                
                
    int sock = 0, valread;
    struct sockaddr_in serv_addr;
    char buffer[1024] = {0};
    if ((sock = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
        printf("\n * Socket creation error * \n");
        exit(-1);
    }
      
    memset(&serv_addr, '0', sizeof(serv_addr));
  
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_port = htons(PORT);
     
    // Convert IPv4 and IPv6 addresses from text to binary form
    if(inet_pton(AF_INET, "127.0.0.1", &serv_addr.sin_addr)<=0) {
        printf("\nInvalid address/ Address not supported \n");
        exit(-1);
    }
  
    if (connect(sock, (struct sockaddr *)&serv_addr, sizeof(serv_addr)) < 0) {
        printf("\nConnection Failed \n");
        exit(-1);
    }
    
    int send_flags = 0; //TODO modify the flags
    send(sock , command.c_str() , command.length() , send_flags); //TODO 
    printf("Command message sent\n");
    valread = read(sock , buffer, 1024);
    if (valread == -1) perror("read error");
    printf("%s\n",buffer);
    return 0;
}

void get_options(std::string& command, std::string& host, std::string& port, int argc, char** argv) {
    extern int optind, opterr;
    extern char *optarg;
    opterr = 1; /* set to 0 to disable error message */
      
    int choice, help = 0, errflag = 0;      
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
}

void usage() {
    std::cout<<"distributed shell client"<<std::endl;
    std::cout<< "usage: dsh [flags] {-c command}, where flags are:"<<std::endl;
    std::cout<<"\t{-c command}    command to execute remotely"<<std::endl;
    std::cout<<"\t{-s host}       host server is on"<<std::endl;
    std::cout<<"\t[-p #]          port server is on (default is 4513)"<<std::endl;
    std::cout<<"\t[-h]            this help message"<<std::endl;
    exit(0);
}