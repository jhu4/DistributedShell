
#ifndef PROJECT_1_H
#define PROJECT_1_H

#include <arpa/inet.h>
#include <stdio.h>
#include <sys/socket.h>
#include <stdlib.h>
#include <netinet/in.h>
#include <unistd.h>
#include <string>
#include <sys/types.h>
#include <netdb.h>
#include <iostream>

void get_options(std::string&, std::string&, std::string&, int, char**);
void free_exit(struct addrinfo* serv);
void usage();
#endif //PROJECT_1_H
