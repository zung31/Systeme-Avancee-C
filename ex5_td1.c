#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>

int main() {
    pid_t p;
    int s;
    p = fork();
    if(p<0){
        perror("Fil n'est pas crée");
        exit(1);
    }
    if(p==0){ // child 1
        printf("who\n");
        execlp("who","who",NULL);
    }
    else{
        wait(&s); // wait fil1, &s sẽ trả lại id của pid vừa xong
        p = fork();
        if(p<0){
            perror("Fil n'est pas crée");
            exit(1);
        }
        if(p==0){ // child 2
            printf("ps\n");
            execlp("ps","ps",NULL);
        }
        else{
            wait(&s);
            p = fork();
            if(p<0){
                perror("Fil n'est pas crée");
                exit(1);
            }
            if(p==0){ // child 3
                printf("ls\n");
                execlp("ls","ls","-l",NULL);
            }
            else{
                wait(&s);
            }
        }
    }

    return 0;
}