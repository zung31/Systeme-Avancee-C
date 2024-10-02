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
    while (read(read_pipe, &lower_bound, sizeof(int)) > 0 && read(read_pipe, &upper_bound, sizeof(int)) > 0) {
        if (lower_bound == -1 && upper_bound == -1) {
            break; // Terminate signal
        }
        char buffer[5096];
        int offset = 0;
        offset += snprintf(buffer + offset, sizeof(buffer) - offset, 
        "    \nChild Process %d: Prime numbers between %d and %d:\n", getpid(), lower_bound, upper_bound);
        // chercher les nombres premiers
        for (int i = lower_bound; i <= upper_bound; i++) {
            if (is_prime(i)) {
                offset += snprintf(buffer + offset, sizeof(buffer) - offset, "%d ", i);
            }
        }
        snprintf(buffer + offset, sizeof(buffer) - offset, "\n");
        if(write(write_pipe, buffer, offset) == -1){
            perror("Error writing tube");
        }
        // Informer le parent que cet enfant est prêt pour plus de travail
        int done_signal = 1;
        write(write_pipe, &done_signal, sizeof(int));
    }
    close(read_pipe); // fermer pipe read
    close(write_pipe); // fermer pipe write
    exit(0);
}

int main(){
    pid_t pid;
    int s;
    int p, n; // nombre de processus et nombre de sous intervalles
    
    printf("!Attention p doit <= n!\nEntrer nombre de processus(p) et nombre de sous intervalles(n): ");
    scanf("%d %d", &p, &n);

    int pipe_to_children[p][2], pipe_from_children[p][2]; // creer p pipes -> correspondant à chaque processus
    // creer pipes
    for (int i = 0; i < p; i++) {
        if (pipe(pipe_to_children[i]) == -1 || pipe(pipe_from_children[i]) == -1) {
            perror("pipe");
            exit(1);
        }
    }

    // process pour les fils
    for (int i = 0; i < p; i++) { // creer un loop pour les p fils
        pid = fork(); // creer fil
        if (pid < 0) { // error creation fil
            perror("Fil n'est pas crée");
            exit(1);
        }
        if (pid == 0) { // child process
            close(pipe_to_children[i][1]); // fermer pipe write
            close(pipe_from_children[i][0]); // fermer pipe read
            fil_process(pipe_to_children[i][0], pipe_from_children[i][1]); // envoyer pipe read de pere et pipe pour ecrire dans fil process
        } else { //pere
            close(pipe_to_children[i][0]); // fermer pipe read
            close(pipe_from_children[i][1]); // fermer pipe write
        }
    }

    // les bound pour chaques fils
    int bounds[n][2];
    int interval_size = 1000 / n; // diviser pour chaque intervalle
    for (int i = 0; i < n; i++) {
        bounds[i][0] = i * interval_size + 1;
        bounds[i][1] = (i + 1) * interval_size;
    }

    // envoyer les bounds à chaque fils
    int current_interval = 0;
    for(int i=0; i < p && current_interval < n; i++){
        write(pipe_to_children[i][1], &bounds[current_interval][0], sizeof(int));
        write(pipe_to_children[i][1], &bounds[current_interval][1], sizeof(int));
        current_interval++;
    }

    // lire les nombres premiers de chaque fil
    int active_children = p;
    while (active_children > 0) { // nombre des fils encore travaillés
        for (int i = 0; i < p; i++) { // loop chaque fil
            int done_signal;
            if (read(pipe_from_children[i][0], &done_signal, sizeof(int)) > 0) {
                char buffer[4096];
                int bytes_read = read(pipe_from_children[i][0], buffer, sizeof(buffer));
                if (bytes_read > 0) { 
                    buffer[bytes_read] = '\0'; // Null-terminate the buffer
                    printf("%s", buffer);
                }
                if (current_interval < n) {
                    write(pipe_to_children[i][1], &bounds[current_interval][0], sizeof(int));
                    write(pipe_to_children[i][1], &bounds[current_interval][1], sizeof(int));
                    current_interval++;
                } else {
                    int terminate_signal[2] = {-1, -1};
                    write(pipe_to_children[i][1], &terminate_signal[0], sizeof(int));
                    write(pipe_to_children[i][1], &terminate_signal[1], sizeof(int));
                    printf("Terminated\n");
                    active_children--;
                }
            }
        }
    }

    // attendre que tous les fils finissent
    for (int i = 0; i < p; i++) {
        wait(&s);
    }
    
    return 0;
}