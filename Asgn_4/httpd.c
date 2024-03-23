#define _GNU_SOURCE
#include "net.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/wait.h>
#include <signal.h>
#include <unistd.h>

#define BUFFER_SIZE 1024
#define PATH_SIZE 256

void reply(FILE *file, FILE *network, int statCode, char *message, long length){
    fprintf(network, "HTTP/1.0 %d %s\r\n", statCode, message);
    //always sepcify text/html as content type regardless of file
    fprintf(network, "Content-Type: text/html\r\n");

    if (length >= 0){
        fprintf(network, "Content-Length: %ld\r\n", (long)length);
    }

    fprintf(network, "\r\n");

    if (file != NULL && length > 0){
        char buffer[BUFFER_SIZE];
        while (fgets(buffer, BUFFER_SIZE, file) != NULL){
            fputs(buffer, network);
        }
    }
}

void handle_request(int nfd){
    FILE *network = fdopen(nfd, "r+");
    char *line = NULL;
    size_t size;
    ssize_t num;

    if (network == NULL)
    {
        perror("fdopen");
        close(nfd);
        return;
    }
    //get request line
    num = getline(&line, &size, network);
    if (num <= 0) {
        reply(NULL, network, 400, "Bad Request", -1);
    } else{
        //get http method (GET)
        char *method = strtok(line, " ");
        //get specific path continuing after the method
        char *path = strtok(NULL, " ");
        if (path && path[0] == '/'){
            path++;
        }

        if (path && strcmp(path, "index.html") == 0){
            FILE *file = fopen(path, "r");
            if (file){
                //use stat sys call to get content;length
                struct stat file_stat;
                if (stat(path, &file_stat) == 0){
                    if (strcmp(method, "GET") == 0){
                        printf("get request processing...\n");
                        reply(file, network, 200, "OK", (long)file_stat.st_size);
                    } else if (strcmp(method, "HEAD") == 0){
                        printf("head request processing...\n");
                        reply(NULL, network, 200, "OK", (long)file_stat.st_size);
                    } else {
                        reply(NULL, network, 501, "Not Implemented", -1);
                    }
                } else {
                    reply(NULL, network, 404, "Not Found", -1);
                }
                fclose(file);

            } else {
                reply(NULL, network, 404, "Not Found", -1);
            }
        } else {
            reply(NULL, network, 404, "Not Found", -1);
        }
    }

    free(line);
    fclose(network);
}

void run_service(int fd)
{
    while (1){
        int nfd = accept_connection(fd);
        if (nfd != -1){
            pid_t pid = fork();
            if (pid == -1){
                printf("error forking child");
                exit(1);
            } else if (pid == 0){
                close(fd);
                printf("child process %d is occuring\n", getpid());
                handle_request(nfd);
                close(nfd);
                exit(0);
            } else if (pid > 0){
                printf("parent processs is occuring\n");
                int status;
                close(nfd);
                waitpid(pid, &status, 0);
            }
        }
    }
}

int main(int argc, char *argv[]){
    if (argc != 2){
        printf("Wrong number of arguments provided");
        exit(1);
    }
    //new part for asgn 4
    int port = atoi(argv[1]);
    if (port < 1024 || port > 65535){
        printf("Port number out of range between 1024-65535");
        exit(1);
    }

    int fd = create_service(port);
    if (fd == -1){
        perror(0);
        exit(1);
    }

    printf("listening on port: %d\n", port);
    run_service(fd);
    close(fd);

    return 0;
}
