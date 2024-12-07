#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/ipc.h>
#include <sys/msg.h>
#include <unistd.h>

#define MSG_SIZE 100

struct msg_buffer {
    long msg_type; // belong to which process?
    char msg_text[MSG_SIZE]; // message
};

int main() {
    key_t key;
    int msgid;
    struct msg_buffer message;

    // generate unique key
    key = ftok("progfile", 65);

    msgid = msgget(key, 0666 | IPC_CREAT); // return id
    if (msgid < 0) {
        perror("msgget");
        exit(1);
    }

    pid_t p = fork();
    if (p < 0) {
        perror("Fil n'est pas crée");
        exit(1);
    }
    if (p == 0) { // fil (process 2)
        msgrcv(msgid, &message, sizeof(message), 1, 0); // Receive message from Process 1
        printf("Processus 2 reçoit : %s\n", message.msg_text);
        // Send message to Process 1
        message.msg_type = 2;
        strcpy(message.msg_text, "je suis le processus 2"); // attach message to msg_text
        msgsnd(msgid, &message, sizeof(message), 0);
    } else { // pere
        // Send message to Process 2
        message.msg_type = 1;
        strcpy(message.msg_text, "je suis le processus 1"); // attach message to msg_text
        msgsnd(msgid, &message, sizeof(message), 0);

        // Wait for Process 2 to send a message back
        msgrcv(msgid, &message, sizeof(message), 2, 0);
        printf("Processus 1 reçoit : %s\n", message.msg_text);

        // Destroy the message queue
        msgctl(msgid, IPC_RMID, NULL);
    }

    return 0;
}