#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>

int main() {
    pid_t p;
    int s, T[2];
    if(pipe(T)==-1){
        perror("error pipe");
        exit(1);
    }
    p = fork();
    if(p<0){
        perror("Fil n'est pas crée");
        exit(1);
    }
    if(p == 0){ //fil
        close(1); // đóng file descriptor tiêu chuẩn đầu ra để chuyển hướng
        dup(T[1]);// chuyển hướng đầu ra của ls
        execlp("ls", "ls", "-l", NULL); // các hàm exec không cần trực tiếp làm việc với pipe, chỉ cần chỉnh đầu ra vào hoi
    }
    else{ // pere
        close(0); // // đóng file descriptor tiêu chuẩn đầu vào
        dup(T[0]); // chuyển hướng đầu vào của ls
        execlp("grep", "grep", "td", NULL); // tìm những file nào có chứa tên td
    }
    return 0;
}