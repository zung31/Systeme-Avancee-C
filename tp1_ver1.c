#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>
#include <stdbool.h>

bool is_prime(int num) {
    if (num <= 1) return false;
    for (int i = 2; i * i <= num; i++) {
        if (num % i == 0) return false;
    }
    return true;
}

// processus de fil
void fil_process(int read_pipe) {
    close(read_pipe + 1); // fermer pipe write

    int lower_bound, upper_bound;
    read(read_pipe, &lower_bound, sizeof(int)); // prendre les min et maxBound
    read(read_pipe, &upper_bound, sizeof(int));

    // creer un string pour contenir le resultat
    char buffer[4096];
    int offset = 0;

    offset += snprintf(buffer + offset, sizeof(buffer) - offset, 
    "Child process %d: Prime numbers between %d and %d:\n", getpid(), lower_bound, upper_bound);

    // chercher les nombres premiers
    for (int i = lower_bound; i <= upper_bound; i++) {
        if (is_prime(i)) {
            offset += snprintf(buffer + offset, sizeof(buffer) - offset, "%d ", i);
        }
    }

    snprintf(buffer + offset, sizeof(buffer) - offset, "\n");
    printf("%s", buffer); // printf resultat

    close(read_pipe); // fermer pipe read
    exit(0);
}

int main() {
    pid_t p;
    int s;
    int pipe1[2],pipe2[2]; // pipe est presente par 2 elements
    //pipe[0] est pipe read
    //pipe[1] est pipe write

    // creer pipe1 et pipe2
    if (pipe(pipe1) == -1 || pipe(pipe2) == -1) {
        perror("pipe");
        exit(1);
    }

    // process pour fil 1
    p = fork();
    if(p<0){
        perror("Fil n'est pas crée");
        exit(1);
    }
    if(p==0){ // child 1
        close(pipe1[1]); // fermer pipe write
        fil_process(pipe1[0]); //envoyer le pipe read dans fil process
    }
    
    // process pour fil 2
    p = fork();
    if(p<0){
        perror("Fil n'est pas crée");
        exit(1);
    }
    if(p==0){ // child 2
        close(pipe2[1]); // fermer pipe write
        fil_process(pipe2[0]); //envoyer le pipe read dans fil process
    }

    // process pour pere
    close(pipe1[0]); // fermer pipe read 
    close(pipe2[0]); // fermer pipe read

    //envoyer les bounds pour fil 1
    int lower_bound1 = 2, upper_bound1 = 500;
    write(pipe1[1], &lower_bound1, sizeof(int));
    write(pipe1[1], &upper_bound1, sizeof(int));

    //envoyer les bounds pour fil 2
    wait(&s);
    int lower_bound2 = 501, upper_bound2 = 1000;
    write(pipe2[1], &lower_bound2, sizeof(int));
    write(pipe2[1], &upper_bound2, sizeof(int));

    // fermer pipe write apres utilisation
    close(pipe1[1]);
    close(pipe2[1]);
    
    wait(&s);
    return 0;
}