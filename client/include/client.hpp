// Zachary Halzel
// Jinan Hu

#ifndef PROJECT_1_H
#define PROJECT_1_H

#include <arpa/inet.h>
#include <stdio.h>
#include <sys/socket.h>
#include <stdlib.h>
#include <netinet/in.h>
#include <unistd.h>
#include <string.h>
#include <sys/types.h>
#include <netdb.h>
#include <iostream>
#include <fstream>
#include <crypt.h>
#include <unordered_map>
#include <sys/time.h>

#define MAX_SIZE 1024

void get_options(std::string&, std::string&, std::string&, int, char**);
void free_exit(struct addrinfo*);
void usage();
std::string user_name();
void send_to_server(int, std::string);
int get_sock(std::string host, std::string port, struct addrinfo* serv);
int unique_number(char* buffer, struct addrinfo* serv);
void receive_from_server(int sock, char* buffer, struct addrinfo* serv);
std::string encrypt(int unique_number);
std::string user_name();
int send_all(int sock, char *buf, int *len);
char* get_hashcode(char* rand);
long get_milliseconds(struct timeval tv);
void write_file(long time);

#endif //PROJECT_1_H
