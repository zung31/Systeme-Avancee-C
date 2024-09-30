#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>
#include <signal.h>
#include<string.h>

int main(){
    pid_t p;
    signal(SIGHUP, SIG_IGN); //thiết lập trình xử lý tín hiệu để bỏ qua tín hiệu SIGHUP
    p = fork();
    if(p<0){
        perror("Fil n'est pas crée");
        exit(1);
    }
    if(p == 0){
        execvp(argv[i], &argv[i]); // execvp được sử dụng để thay thế tiến trình hiện tại bằng một tiến trình mới
        // argv[1] là lệnh và &argv[1] là danh sách các đối số cho lệnh đó.
    }

    return 0;
}