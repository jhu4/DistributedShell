#include "client.hpp"

int main(int argc, char** argv) {
    std::cout<<"hello world"<<std::endl;
    // Client side C/C++ program to demonstrate Socket programming

    // struct sockaddr_in address;
    std::string command;
    std::string host;
    int port = get_options(command, host, argc, argv);
    
    std::cout << "Host server is " << host << " Command is" << command;
    std::cout << "Port is "<< port << std::endl; 
    
//     int sock = 0, valread;
//     struct sockaddr_in serv_addr;
//     std::string hello = "Hello from client";
//     char buffer[1024] = {0};
//     if ((sock = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
//         printf("\n Socket creation error \n");
//         return -1;
//     }
//   
//     memset(&serv_addr, '0', sizeof(serv_addr));
//   
//     serv_addr.sin_family = AF_INET;
//     serv_addr.sin_port = htons(PORT);
//       
//     // Convert IPv4 and IPv6 addresses from text to binary form
//     if(inet_pton(AF_INET, "127.0.0.1", &serv_addr.sin_addr)<=0) {
//         printf("\nInvalid address/ Address not supported \n");
//         return -1;
//     }
//   
//     if (connect(sock, (struct sockaddr *)&serv_addr, sizeof(serv_addr)) < 0) {
//         printf("\nConnection Failed \n");
//         return -1;
//     }
//     
//     int send_flags = 0; //TODO modify the flags
//     send(sock , hello.c_str() , hello.length() , send_flags); //TODO 
//     printf("Hello message sent\n");
//     valread = read( sock , buffer, 1024);
//     printf("%s\n",buffer );
    return 0;
}

int get_options(std::string& command, std::string& host, int argc, char** argv) {
    extern int optind, opterr;
    extern char *optarg;
    opterr = 1; /* set to 0 to disable error message */
      
    int choice, help = 0, errflag = 0, port = 4513;      
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
                port = atoi(strdup(optarg));
                
                if (!port) {
                    perror(optarg);
                    exit(-1);
                }
                
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
    return port;
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