#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>
#include <signal.h>
#include<string.h>

void f1(int s){

}

void f2(int s){

}

int main(){
    pid_t p;
    p = fork();
    if(p<0){
        perror("Fil n'est pas crée");
        exit(1);
    }
    if(p == 0){
        
    }
    return 0;
} 