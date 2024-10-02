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

int main(){
    pid_t p;
    int s;
    int pipe1[2],pipe2[2],pipe3[2],pipe4[2],pipe5[2],pipe6[2],pipe7[2],pipe8[2],pipe9[2],pipe10[2];

    // creer pipe1->10
    if (pipe(pipe1) == -1 || pipe(pipe2) == -1 || pipe(pipe3) == -1 || pipe(pipe4) == -1 || pipe(pipe5) == -1 || pipe(pipe6) == -1 
    || pipe(pipe7) == -1 || pipe(pipe8) == -1 || pipe(pipe9) == -1 || pipe(pipe10) == -1) {
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

    // process pour fil 3
    p = fork();
    if(p<0){
        perror("Fil n'est pas crée");
        exit(1);
    }
    if(p==0){ // child 3
        close(pipe3[1]); // fermer pipe write
        fil_process(pipe3[0]); //envoyer le pipe read dans fil process
    }
    
    // process pour fil 4
    p = fork();
    if(p<0){
        perror("Fil n'est pas crée");
        exit(1);
    }
    if(p==0){ // child 4
        close(pipe4[1]); // fermer pipe write
        fil_process(pipe4[0]); //envoyer le pipe read dans fil process
    }

    // process pour fil 5
    p = fork();
    if(p<0){
        perror("Fil n'est pas crée");
        exit(1);
    }
    if(p==0){ // child 5
        close(pipe5[1]); // fermer pipe write
        fil_process(pipe5[0]); //envoyer le pipe read dans fil process
    }
    
    // process pour fil 6
    p = fork();
    if(p<0){
        perror("Fil n'est pas crée");
        exit(1);
    }
    if(p==0){ // child 6
        close(pipe6[1]); // fermer pipe write
        fil_process(pipe6[0]); //envoyer le pipe read dans fil process
    }

    // process pour fil 7
    p = fork();
    if(p<0){
        perror("Fil n'est pas crée");
        exit(1);
    }
    if(p==0){ // child 7
        close(pipe7[1]); // fermer pipe write
        fil_process(pipe7[0]); //envoyer le pipe read dans fil process
    }
    
    // process pour fil 8
    p = fork();
    if(p<0){
        perror("Fil n'est pas crée");
        exit(1);
    }
    if(p==0){ // child 8
        close(pipe8[1]); // fermer pipe write
        fil_process(pipe8[0]); //envoyer le pipe read dans fil process
    }

    // process pour fil 9
    p = fork();
    if(p<0){
        perror("Fil n'est pas crée");
        exit(1);
    }
    if(p==0){ // child 9
        close(pipe9[1]); // fermer pipe write
        fil_process(pipe9[0]); //envoyer le pipe read dans fil process
    }
    
    // process pour fil 10
    p = fork();
    if(p<0){
        perror("Fil n'est pas crée");
        exit(1);
    }
    if(p==0){ // child 10
        close(pipe10[1]); // fermer pipe write
        fil_process(pipe10[0]); //envoyer le pipe read dans fil process
    }

    // process pour pere
    close(pipe1[0]); // fermer pipe read 
    close(pipe2[0]); // fermer pipe read
    close(pipe3[0]);  
    close(pipe4[0]);  
    close(pipe5[0]);  
    close(pipe6[0]);  
    close(pipe7[0]);  
    close(pipe8[0]); 
    close(pipe9[0]);  
    close(pipe10[0]);  

    //envoyer les bounds pour fil 1
    int lower_bound1 = 2, upper_bound1 = 100;
    write(pipe1[1], &lower_bound1, sizeof(int));
    write(pipe1[1], &upper_bound1, sizeof(int));

    //envoyer les bounds pour fil 2
    wait(&s);
    int lower_bound2 = 101, upper_bound2 = 200;
    write(pipe2[1], &lower_bound2, sizeof(int));
    write(pipe2[1], &upper_bound2, sizeof(int));

    //envoyer les bounds pour fil 3
    lower_bound1 = 201, upper_bound1 = 300;
    write(pipe3[1], &lower_bound1, sizeof(int));
    write(pipe3[1], &upper_bound1, sizeof(int));

    //envoyer les bounds pour fil 4
    wait(&s);
    lower_bound2 = 301, upper_bound2 = 400;
    write(pipe4[1], &lower_bound2, sizeof(int));
    write(pipe4[1], &upper_bound2, sizeof(int));

    //envoyer les bounds pour fil 5
    lower_bound1 = 401, upper_bound1 = 500;
    write(pipe5[1], &lower_bound1, sizeof(int));
    write(pipe5[1], &upper_bound1, sizeof(int));

    //envoyer les bounds pour fil 6
    wait(&s);
    lower_bound2 = 501, upper_bound2 = 600;
    write(pipe6[1], &lower_bound2, sizeof(int));
    write(pipe6[1], &upper_bound2, sizeof(int));

    //envoyer les bounds pour fil 7
    lower_bound1 = 601, upper_bound1 = 700;
    write(pipe7[1], &lower_bound1, sizeof(int));
    write(pipe7[1], &upper_bound1, sizeof(int));

    //envoyer les bounds pour fil 8
    wait(&s);
    lower_bound2 = 701, upper_bound2 = 800;
    write(pipe8[1], &lower_bound2, sizeof(int));
    write(pipe8[1], &upper_bound2, sizeof(int));

    //envoyer les bounds pour fil 9
    lower_bound1 = 801, upper_bound1 = 900;
    write(pipe9[1], &lower_bound1, sizeof(int));
    write(pipe9[1], &upper_bound1, sizeof(int));

    //envoyer les bounds pour fil 10
    wait(&s);
    lower_bound2 = 901, upper_bound2 = 1000;
    write(pipe10[1], &lower_bound2, sizeof(int));
    write(pipe10[1], &upper_bound2, sizeof(int));

    // fermer pipe write apres utilisation
    close(pipe1[1]);
    close(pipe2[1]);
    close(pipe3[1]);
    close(pipe4[1]);
    close(pipe5[1]);
    close(pipe6[1]);
    close(pipe7[1]);
    close(pipe8[1]);
    close(pipe9[1]);
    close(pipe10[1]);
    
    wait(&s);
    return 0;
}