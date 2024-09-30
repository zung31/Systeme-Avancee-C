#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>

int main() {
    pid_t p;
    int s, T[2];
    char temps[8];
    if(pipe(T)==-1){
        perror("error pipe");
        exit(1);
    }
    p = fork();
    if(p<0){
        perror("Fil n'est pas crée");
        exit(1);
    }
    if(p==0){ // child 1
        close(T[0]); // đóng đầu đọc
        write(T[1],"Bonjour", 7); // size of string
        close(T[1]);
        exit(0);
    }
    else{
        close(T[1]);
        read(T[0], temps, 8);
        write(1, temps, 7); // in 7 bytes của chuỗi temps ra màn hình, 1 là file descriptor cho stdout
        close(T[0]);
        wait(&s);
    }

    return 0;
}