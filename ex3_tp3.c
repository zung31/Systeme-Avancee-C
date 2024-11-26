#include <stdio.h>
#include <stdlib.h>
#include <signal.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <sys/wait.h>
#include <unistd.h>
#include <sys/types.h>

#define SHM_SIZE sizeof(int)

void handle_signal(int sig) {}

int main(){
    key_t key = ftok("shmfile", 12); // unique key
    int shmid;
    int *shared_memory;

    // creer memoire commun
     if ((shmid = shmget(key, SHM_SIZE, 0666 | IPC_CREAT)) == -1) {
        perror("shmget");
        exit(1);
    }

    pid_t p = fork();
    if (p < 0) {
        perror("Fil n'est pas crée");
        exit(1);
    }
    if (p == 0) { // fil
        signal(SIGUSR1, handle_signal); // attach handle avec SIGUSR1
        if ((shared_memory = (int *)shmat(shmid, NULL, 0)) == (void *)-1) {
            perror("shmat fil");
            exit(1);
        }
        for(int i = 1; i <= 10; i++){
            pause(); // attend pere
            printf("Fil: Nombre de shared memory: %d\n", *shared_memory); // debug
            kill(getppid(), SIGUSR1); // envoyer signal à pere
        }
        exit(0);
    }
    else{ // pere
        if ((shared_memory = (int *)shmat(shmid, NULL, 0)) == (void *)-1) {
            perror("shmat pere");
            exit(1);
        }
        signal(SIGUSR1, handle_signal); // attach handle avec SIGUSR1
        for(int i = 1; i <= 10; i++){
            *shared_memory = i;
            printf("Pere: Nombre de shared memory: %d\n", *shared_memory);
            kill(p, SIGUSR1); // envoyer signal à fil
            pause(); // attend fil
        }
        wait(NULL);
        shmdt(shared_memory);
        shmctl(shmid, IPC_RMID, NULL);
    }
    return 0;
}