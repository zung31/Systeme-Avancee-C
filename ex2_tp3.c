#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>
#include <sys/sem.h>

#define SHM_SIZE sizeof(int) // kích thước một số nguyên

union semun { 
    int val; // initialise valeur pour semaphore
    struct semid_ds *buf;   
    unsigned short *array; 
};

void sem_op(int semid, int sem_num, int op) {
    struct sembuf sop;
    sop.sem_num = sem_num; // semaphore 0 ou 1
    sop.sem_op = op; // Giá trị điều chỉnh semaphore (-1 hoặc +1)
    sop.sem_flg = 0; // no flag
    if (semop(semid, &sop, 1) == -1) {
        perror("semop");
        exit(1);
    }
}

int main(){
    key_t key = ftok("shmfile", 12); // unique key
    int shmid, semid;
    int *shared_memory;

    // creer memoire commun
    if ((shmid = shmget(key, SHM_SIZE, 0666 | IPC_CREAT)) == -1) {
        perror("shmget");
        exit(1);
    }

    // creer semaphore
    if ((semid = semget(key, 2, 0666 | IPC_CREAT)) == -1) {
        perror("semget");
        exit(1);
    }
    union semun sem_union;
    sem_union.val = 1; // (sem_write): ecrire d'abord
    if (semctl(semid, 0, SETVAL, sem_union) == -1) {
        perror("semctl (sem_write)");
        exit(1);
    }
    sem_union.val = 0; // (sem_read): lire apres
    if (semctl(semid, 1, SETVAL, sem_union) == -1) {
        perror("semctl (sem_read)");
        exit(1);
    }

    pid_t p = fork();
    if (p < 0) {
        perror("Fil n'est pas crée");
        exit(1);
    }
    if (p == 0) { // fil
        if ((shared_memory = (int *)shmat(shmid, NULL, 0)) == (void *)-1) {
            perror("shmat fil");
            exit(1);
        }
        for (int i = 1; i <= 10; i++) {
            // wait pere
            sem_op(semid, 1, -1); // P(sem_read) 
            printf("Fil: Nombre de shared memory: %d\n", *shared_memory); // debug
            sem_op(semid, 0, 1);  // V(sem_write)
        }
    }
    else{ // pere
        if ((shared_memory = (int *)shmat(shmid, NULL, 0)) == (void *)-1) {
            perror("shmat pere");
            exit(1);
        }
        for (int i = 1; i <= 10; i++) {
            sem_op(semid, 0, -1); // P(sem_write) 
            *shared_memory = i;
            printf("Pere: Nombre de shared memory: %d\n", *shared_memory); // debug
            sem_op(semid, 1, 1);  // V(sem_read)
        }

        wait(NULL); // wait fil
        shmdt(shared_memory); // tháo bộ nhớ dùng chung
        // giải phóng bộ nhớ dùng chung và semaphores
        shmctl(shmid, IPC_RMID, NULL);
        semctl(semid, 0, IPC_RMID);
    }
    // p(v1) pere v(v2) p(v2) fil v(v1) -> return to pere 
    return 0;
}