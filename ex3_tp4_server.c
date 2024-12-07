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

    // Generate unique key
    key = ftok("progfile", 65);

    msgid = msgget(key, 0666 | IPC_CREAT);
    if (msgid < 0) {
        perror("msgget");
        exit(1);
    }

    while (1) {
        // Receive message from client (msg_type = 1)
        msgrcv(msgid, &message, sizeof(message), 1, 0);
        printf("Server reçoit : %s\n", message.msg_text);

        // Parse the message: client_pid, num1, num2, operation
        long client_pid;
        int num1, num2, operation;
        sscanf(message.msg_text, "%ld %d %d %d", &client_pid, &num1, &num2, &operation);

        // Perform the requested operation
        int result;
        switch (operation) {
            case 1: // Addition
                result = num1 + num2;
                printf("Server calcule : %d + %d = %d\n", num1, num2, result);
                break;
            case 2: // Subtraction
                result = num1 - num2;
                printf("Server calcule : %d - %d = %d\n", num1, num2, result);
                break;
            case 3: // Multiplication
                result = num1 * num2;
                printf("Server calcule : %d * %d = %d\n", num1, num2, result);
                break;
            case 4: // Division
                if (num2 == 0) {
                    printf("Erreur: Division par zéro non autorisée.\n");
                    result = 0;
                } else {
                    result = num1 / num2;
                    printf("Server calcule : %d / %d = %d\n", num1, num2, result);
                }
                break;
            default:
                printf("Opération invalide.\n");
                result = 0;
                break;
        }

        // Send result back to client (msg_type = client_pid)
        message.msg_type = client_pid;
        snprintf(message.msg_text, MSG_SIZE, "%d", result);
        msgsnd(msgid, &message, sizeof(message), 0);

        // Stop if 0 0
        if (num1 == 0 && num2 == 0) {
            printf("Server arrêté.\n");
            break;
        }
    }

    // Destroy the message queue
    msgctl(msgid, IPC_RMID, NULL);

    return 0;
}
