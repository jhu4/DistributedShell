// Zachary Halzel
// CS 4513 Project 1

#include "copy_time.h"

int main(int argc, char** argv) {
    char* file_name;
    if (argv[1]) file_name = strdup(argv[1]);
    else exit(-1);
        
    printf("Hello tester!, testing with file [%s]\n", file_name);
    
    char* dumpster_path = getenv("DUMPSTER");
    
    strcat(dumpster_path, "/");
    strcat(dumpster_path, file_name);
    printf("%s", dumpster_path);
    
    FILE *data;
    char* data_text = strdup(file_name);
    strcat(data_text, ".data.txt");
    data = fopen(data_text, "w");
    if (!data) {
        printf("Unable to open the swap file!!\n");
        exit(-1);
    }
    fprintf(data,"Hello tester!, testing with file [%s]\n", file_name);
    
    
    double bytes = get_bytes(file_name) * 1.0;
    printf("bytes: %f\n", bytes);
    fprintf(data, "bytes: %f\n", bytes);
    
    double seconds, bps, total_bps = 0;
    for (int i = 0; i < 100; i++) {
        printf("Test #%d ", i);
        seconds = move_file(file_name, dumpster_path) / 1000.0;
        unlink(dumpster_path);
        bps = bytes/seconds;
        printf("bps: %f\n", bps);
        fprintf(data, "%f\n", bps);
        total_bps += bps;
    }
    fclose(data);
    
    printf("average bytes per second %f\n", total_bps / 100);
}

long get_milliseconds(struct timeval tv) {
    long seconds = tv.tv_sec;
    long microseconds = tv.tv_usec;
    return (seconds * 1000) + (microseconds / 1000);
}

void path_append(char* buf, char* path, char* file) {
    buf[0] = '\0';
    strcpy(buf, path);
    strcat(buf, "/");
    strcat(buf, file);
}


long move_file(char* old_path, char* new_path) {
    struct timeval tv;
    long start_time, end_time;

    if (gettimeofday(&tv, NULL) != 0) printf("Error computing time");
    start_time = get_milliseconds(tv);

    // Based on code found @ https://www.sanfoundry.com/c-program-copy-file/
    FILE *old_file, *new_file;
    char c;
    int pos;
    if (!(old_file = fopen(old_path,"r"))) {    
        printf("\nFile cannot be opened\n");
        return 0;
    } 
    if(!(new_file = fopen(new_path, "w"))) {
        printf("\nfopen error\n");
        return 0;
    }  
    

    fseek(old_file, 0L, SEEK_END); // file pointer at end of file
    pos = ftell(old_file);
    
    fseek(old_file, 0L, SEEK_SET); // file pointer set at start
    while (pos--) {
        c = fgetc(old_file);  // copying file character by character
        fputc(c, new_file);
    }   
    
    fclose(old_file);
    fclose(new_file);
    
    sync();
    
    if (gettimeofday(&tv, NULL) != 0) printf("Error computing time");
    end_time = get_milliseconds(tv);
    
    return (end_time - start_time);
}

int get_bytes(char* file_name) {
    int ret;			/* return value */
    struct stat s;		/* struct to hold file stats */

    ret = stat(file_name, &s);
  
    if (ret != 0) {
        perror("get_stat Failed:");
        exit(ret);
    }
    return s.st_size;
}
