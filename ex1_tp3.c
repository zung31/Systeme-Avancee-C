#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>

#define SHM_SIZE 1024 // 1kb

int main(){
    key_t key = ftok("shmfile", 12); // unique key
    int shmid;

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
    // con sẽ đọc dữ liệu từ đoạn bộ nhớ dùng chung.
        char *shared_memory = (char *)shmat(shmid, NULL, 0); // Gắn cùng đoạn bộ nhớ dùng chung
        if (shared_memory == (char *)(-1)) {
            perror("shmat");
            exit(1);
        }
        printf("Message de shared memory: %s\n", shared_memory); // debug
        // Tháo bộ nhớ dùng chung
        shmdt(shared_memory);
    }
    else{ // pere
    // pere sẽ quản lý bộ nhớ và ghi dữ liệu vào đoạn bộ nhớ dùng chung
        char *shared_memory = (char *)shmat(shmid, NULL, 0); // Gắn bộ nhớ
        if (shared_memory == (char *)(-1)) {
            perror("shmat");
            exit(1);
        }
        // Ghi vào bộ nhớ
        strcpy(shared_memory, "Bonjour");
        printf("Message dans shared memory: Bonjour\n"); // debug
        wait(NULL); // gọi để chờ tiến trình con kết thúc
        // Xóa bộ nhớ dùng chung
        shmctl(shmid, IPC_RMID, NULL);
    }
    return 0;
}