
#ifndef PROJECT_1_H
#define PROJECT_1_H

#include <iostream>
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
#include <unordered_map>
#include <vector>

#define PORT "4513"
#define MAX_SIZE 1024

void usage();
void server_routine(int server_fd);
void get_options(char* directory, std::string& port, int argc, char** argv);
void change_dir(std::string directory);
void sigchld_handler(int s);
char* get_hashcode(char* rand, std::string username);
void set_hints(struct addrinfo &hints);
void receive_from_client(int sock, char* buffer);
int authenticate(std::string user_name, std::string hashed_password, std::string random_num);
void execute(char* cmd);

#endif //PROJECT_1_H
