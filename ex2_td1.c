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
    if(p==0){ // child
        for(int i=51;i<=100;i++){
            printf("%d ",i);
        }
        printf("\n");
        exit(0);
    }
    else{
        wait(&s);
        for(int i=1;i<=50;i++){
            printf("%d ",i);
        }
        printf("\n");
    }
    return 0;
}