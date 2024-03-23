#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <string.h>
#include <sys/wait.h>

int main(void){
    //adapted from lec 14 IPC_pip.c github code
    int parent_to_first[2];
    int first_to_second[2];
    int second_to_parent[2];

    if (pipe(parent_to_first) == -1 ||
        pipe(first_to_second) == -1 ||
        pipe(second_to_parent) == -1) {
        perror("Pipe creation failed");
        exit(EXIT_FAILURE);
    }

    pid_t first_child_pid = fork();
    if (first_child_pid == -1) 
    {
        perror("Fork failed");
        exit(EXIT_FAILURE);
    }

    //want to read from parent and write to second
    if (first_child_pid == 0){
        //close write of parent to first
        close(parent_to_first[1]);
        //close read of first to second
        close(first_to_second[0]);
        //close both for second to parent since not using
        close(second_to_parent[0]);
        close(second_to_parent[1]);

        int num;
        while (read(parent_to_first[0], &num, sizeof(int)) > 0){
            num = num * num;
            write(first_to_second[1], &num, sizeof(int));
        }

        //close remaining ends
        close(parent_to_first[0]);
        close(first_to_second[1]);
        exit(0);
    } else {
        //parent forks for second child
        pid_t second_child_pid = fork();
        if (second_child_pid == -1) {
            perror("Fork failed");
            exit(EXIT_FAILURE);
        }

        //if this is second child
        if (second_child_pid == 0) {
            close(parent_to_first[0]);
            close(parent_to_first[1]);
            close(first_to_second[1]);
            close(second_to_parent[0]);

            int num;
            while (read(first_to_second[0], &num, sizeof(int)) > 0){
                num += 1;
                write(second_to_parent[1], &num, sizeof(int));
            }

            //close remaining ends
            close(first_to_second[0]);
            close(second_to_parent[1]);
            exit(0);
        } else {
            //parent process
            //write to first after reading second 
            close(parent_to_first[0]);
            close(first_to_second[0]);
            close(first_to_second[1]);
            close(second_to_parent[1]);

            int num;
            printf("Enter Integer: ");
            while (scanf("%d", &num) != EOF){
                write(parent_to_first[1], &num, sizeof(int));
                read(second_to_parent[0], &num, sizeof(int));
                printf("Result: %d\n", num);
                printf("Enter Integer: ");
            }

            close(parent_to_first[1]);
            close(second_to_parent[0]);

            int status;
            waitpid(first_child_pid, &status, 0);
            printf("first child is done\n");
            waitpid(second_child_pid, &status, 0);
            printf("second child is done\n");
            
        }
    }
    return 0;
}
