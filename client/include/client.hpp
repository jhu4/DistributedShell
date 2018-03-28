
#ifndef PROJECT_1_H
#define PROJECT_1_H

#include <arpa/inet.h>
#include <stdio.h>
#include <sys/socket.h>
#include <stdlib.h>
#include <netinet/in.h>
#include <unistd.h>
#include <string>

int get_options(std::string& command, std::string& host, int argc, char** argv);

void usage();

#define PORT 8080

#include <iostream>

#endif //PROJECT_1_H
