#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>

int main() {
    int num_clients = 3; // numero de client
    pid_t pid;
    char client_program[50]; // nom de programme

    for (int i = 1; i <= num_clients; ++i) {
        snprintf(client_program, sizeof(client_program), "./ex3_tp4_client%d", i); // gắn nom programme
        
        pid = fork();
        if (pid < 0) {
            perror("fork");
            exit(EXIT_FAILURE);
        } else if (pid == 0) {
            // fil: run client program
            execl(client_program, client_program, (char *)NULL);
            // si erreur
            perror("execl");
            exit(EXIT_FAILURE);
        }
    }

    // wait tous les fils finissent
    for (int i = 0; i <= num_clients; ++i) {
        wait(NULL);
    }

    return 0;
}