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

    msgid = msgget(key, 0666 | IPC_CREAT); 
    if (msgid < 0) {
        perror("msgget");
        exit(1);
    }

    while (1) {
        // Receive message from client (msg_type = 1)
        msgrcv(msgid, &message, sizeof(message), 1, 0);
        printf("Server reçoit : %s\n", message.msg_text);

        // Tách hai số từ message nhận được
        int num1, num2;
        sscanf(message.msg_text, "%d %d", &num1, &num2);

        // Sum
        int sum = num1 + num2;
        printf("Server calcule : %d + %d = %d\n", num1, num2, sum);

        // Send result to client (msg_type = 2)
        message.msg_type = 2;
        snprintf(message.msg_text, MSG_SIZE, "%d", sum);
        msgsnd(msgid, &message, sizeof(message), 0);

        // Condition pour s'arreter
        if (num1 == 0 && num2 == 0) {
            printf("Server arrêté.\n");
            break;
        }
    }

    // Hủy message queue
    msgctl(msgid, IPC_RMID, NULL);

    return 0;
}