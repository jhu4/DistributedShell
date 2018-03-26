//
// Created by Zach Halzel
//

#ifndef PROJECT_1_H
#define PROJECT_1_H
#include <stdio.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <getopt.h>
#include <dirent.h>
#include <errno.h>
#include <libgen.h>
#include <utime.h>
#include <fcntl.h>
#include <sys/time.h>

void path_append(char* buf, char* path, char* file);
long get_milliseconds(struct timeval tv);
long move_file(char* old_path, char* new_path);
int get_bytes(char* file_name);

#endif //PROJECT_1_H
