#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/ipc.h>
#include <sys/msg.h>
#include <unistd.h>

#define MSG_SIZE 100

struct msg_buffer {
    long msg_type; // type = PID du client
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
        // Enter 2 numbers and operation
        int num1, num2, operation;
        long client_pid = getpid(); // Unique PID for the client
        printf("1: Addition; 2: Soustraction, 3: Multiplication, 4: Division\n");
        printf("Client %ld: Entrez deux nombres entiers (ou 0 0 pour quitter) et no operation: ", client_pid);
        scanf("%d %d %d", &num1, &num2, &operation);

        // Prepare message to send to server
        message.msg_type = 1; // All clients send to server with msg_type = 1
        snprintf(message.msg_text, MSG_SIZE, "%ld %d %d %d", client_pid, num1, num2, operation);
        msgsnd(msgid, &message, sizeof(message), 0);

        // Receive result from server (msg_type = client_pid)
        msgrcv(msgid, &message, sizeof(message), client_pid, 0);
        printf("Client reçoit : Résultat = %s\n", message.msg_text);

        // Stop if 0 0
        if (num1 == 0 && num2 == 0) {
            printf("Client arrêté.\n");
            break;
        }
    }

    return 0;
}
