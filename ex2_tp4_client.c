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

    key = ftok("progfile", 65);

    msgid = msgget(key, 0666 | IPC_CREAT);
    if (msgid < 0) {
        perror("msgget");
        exit(1);
    }

    while (1) {
        // Enter 2 num
        int num1, num2;
        printf("Client : Entrez deux nombres entiers (ou 0 0 pour quitter) : ");
        scanf("%d %d", &num1, &num2);

        // Send to server (msg_type = 1)
        message.msg_type = 1;
        snprintf(message.msg_text, MSG_SIZE, "%d %d", num1, num2);
        msgsnd(msgid, &message, sizeof(message), 0);

        // Receive result from server (msg_type = 2)
        msgrcv(msgid, &message, sizeof(message), 2, 0);
        printf("Client reçoit : Résultat = %s\n", message.msg_text);

        // Stop if 0 0
        if (num1 == 0 && num2 == 0) {
            printf("Client arrêté.\n");
            break;
        }
    }

    return 0;
}