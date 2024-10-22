#include <stdio.h>
#include <unistd.h>
#include <signal.h>
#include <stdlib.h>

volatile sig_atomic_t counter = 0;
pid_t child_pid; // Khai báo biến toàn cục để lưu trữ PID của tiến trình con

void handle_ping(int sig) {
    counter++;
    printf("ping %d ", counter);
    fflush(stdout); // Đảm bảo rằng giá trị được in ra ngay lập tức
    kill(getppid(), SIGUSR1); // Gửi tín hiệu SIGUSR1 đến tiến trình cha (pong)
}

void handle_pong(int sig) {
    counter++;
    printf("pong %d ", counter);
    fflush(stdout);
    kill(child_pid, SIGUSR2); // Gửi tín hiệu SIGUSR2 đến tiến trình con (ping)
}

int main() {
    pid_t pid;
    pid = fork();
    if (pid < 0) {
        perror("fork");
        exit(1);
    }
    if (pid == 0) { // child (ping)
        signal(SIGUSR2, handle_ping); 
        for (;;) {
            pause(); // Chờ tín hiệu SIGUSR2
        }
    } 
    else { // pere (pong)
        child_pid = pid; // PID de fil
        signal(SIGUSR1, handle_pong); 
        sleep(1); 
        for (int i = 0; i < 10; i++) {
            kill(child_pid, SIGUSR2); // envoyer SIGUSR2 à fil
            pause(); // attendre SIGUSR1 de fil
        }
        kill(child_pid, SIGKILL); // terminer fil à la fin
    }

    return 0;
}
