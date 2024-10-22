#include <stdio.h>
#include <unistd.h>
#include <signal.h>
#include <stdlib.h>

volatile sig_atomic_t counter = 0;

void handle_ping(int sig) {
    counter++;
    printf("ping %d ", counter);
    fflush(stdout); // print dans ecran immediatement
    kill(getppid(), SIGUSR1); // senvoyer signal à pere (pong)
}

void handle_pong(int sig) {
    counter++;
    printf("pong %d ", counter);
    fflush(stdout);
    kill(getpid() + 1, SIGUSR2); // envoyer signal à child (ping)
}

int main() {
    pid_t pid;
    pid = fork();
    if (pid < 0) {
        perror("fork");
        exit(1);
    }
    if (pid == 0) { // fil (ping)
        signal(SIGUSR2, handle_ping); // thiết lập hàm xử lí cho SIGUSR2
        for (;;) {
            pause(); // wait for SIGUSR2
        }
    } 
    else { // pere (pong)
        int i = 0;
        signal(SIGUSR1, handle_pong); // thiết lập hàm xử lí cho SIGUSR1
        sleep(1);
        //kill(pid, SIGUSR2); // commencer par envoyer signal à fil (ping)
        // infinite loop
        // for (;;) {
        //     pause(); // wait for SIGUSR1
        // }
        for (int i = 0; i < 10; i++) {
            kill(pid, SIGUSR2); 
            pause(); 
        }
        kill(pid, SIGKILL);
    }

    return 0;
}