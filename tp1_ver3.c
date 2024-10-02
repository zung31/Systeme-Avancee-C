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
void fil_process(int read_pipe, int write_pipe) {
    int lower_bound, upper_bound;
    read(read_pipe, &lower_bound, sizeof(int)); // prendre les min et maxBound
    read(read_pipe, &upper_bound, sizeof(int));

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
    if(write(write_pipe, buffer, sizeof(buffer)) == -1){
        printf("Error writing tube");
    }

    close(read_pipe); // fermer pipe read
    close(write_pipe); // fermer pipe write
    exit(0);
}

int main(){
    pid_t p;
    int s;
    int pipe_to_children[10][2], pipe_from_children[10][2]; // 2 arrays: 1 pour de pere à fils, l'autre pour de fils à pere

    // creer pipes
    for (int i = 0; i < 10; i++) {
        if (pipe(pipe_to_children[i]) == -1 || pipe(pipe_from_children[i]) == -1) {
            perror("pipe");
            exit(1);
        }
    }

    // process pour les fils
    for (int i = 0; i < 10; i++) { // creer un loop pour les 10 fils
        p = fork(); // creer fil
        if (p < 0) { // error creation fil
            perror("Fil n'est pas crée");
            exit(1);
        }
        if (p == 0) { // child process
            close(pipe_to_children[i][1]); // fermer pipe write
            close(pipe_from_children[i][0]); // fermer pipe read
            fil_process(pipe_to_children[i][0], pipe_from_children[i][1]); // envoyer pipe read de pere et pipe pour ecrire dans fil process
        } else { //pere
            close(pipe_to_children[i][0]); // fermer pipe read
            close(pipe_from_children[i][1]); // fermer pipe write
        }
    }

    // les bound pour chaques fils
    int bounds[10][2] = {
        {2, 100}, {101, 200}, {201, 300}, {301, 400}, {401, 500},
        {501, 600}, {601, 700}, {701, 800}, {801, 900}, {901, 1000}
    };

    // envoyer les bounds à chaque fils
    for(int i=0; i < 10; i++){
        write(pipe_to_children[i][1], &bounds[i][0], sizeof(int));
        write(pipe_to_children[i][1], &bounds[i][1], sizeof(int));
        close(pipe_to_children[i][1]); // fermer pipe write apres utilisation
    }

    // lire les nombres premiers de chaque fil
    for (int i = 0; i < 10; i++) { // loop chaque fil
        char buffer[4096];
        if(read(pipe_from_children[i][0], buffer, sizeof(buffer)) != -1) { // loop des chaines d'un fil jusqu'à -1
            printf("%s ",buffer);
        }
        close(pipe_from_children[i][0]); // fermer pipe read apres utilisation
    }

    
    // attendre que tous les fils finissent
    for (int i = 0; i < 10; i++) {
        wait(&s);
    }
    
    return 0;
}