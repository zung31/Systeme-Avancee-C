#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/wait.h>
#include <unistd.h>
#include <fcntl.h>
#include <errno.h>

volatile sig_atomic_t s = 0, m = 0, h = 0; 
volatile sig_atomic_t pidM, pidH; // PID pour minutes et heures

int pipeS[2], pipeM[2], pipeH[2];  // les 3 pipes pour 3 processus

void affiche(int sec, int min, int hour) {
  printf("Heure: %02d:%02d:%02d\n", hour, min, sec);
  fflush(stdout); // Đảm bảo rằng giá trị được in ra ngay lập tức
}

// signum nhận giá trị là số hiệu của tín hiệu mà tiến trình nhận được như sigalrm, sigint,sigusr1,..
void fs(int signum) {
    if (write(pipeS[1], (const void *)&s, sizeof(s)) == -1) {
        perror("Error writing to pipeS");
        exit(1);
    }
    s++;
    
    if (s == 60) {
        s = 0;
        kill(pidM, SIGUSR1);  // Gửi tín hiệu cho quá trình M -> gửi SIGUSR1 cho quá trình pidM
    }
    alarm(1);
}

void fm(int signum) {
    m++;
    if (write(pipeM[1], (const void *)&m, sizeof(m)) == -1) {
        perror("Error writing to pipeM");
        exit(1);
    }
    
    if (m == 60) {
        m = 0;
        kill(pidH, SIGUSR2);
    }
}

void fh(int signum) {
    h++;
    write(pipeH[1], (const void *)&h, sizeof(h));
    if (h == 24) {
        h = 0;
    }
}

// fs ở cha, fm ở fil và fh ở fil của fil
int main() {
    pid_t p;
    // creer pipes
    if (pipe(pipeS) == -1 || pipe(pipeM) == -1 || pipe(pipeH) == -1) {
        perror("pipe");
        exit(1);
    }

    p = fork();
    if (p < 0) {
        perror("Fil n'est pas crée");
        exit(1);
    }
    if (p == 0) { // fil 
        p = fork();
        if (p < 0) {
            perror("Fil n'est pas crée");
            exit(1);
        }
        if (p == 0) { // fil du fil
            close(pipeH[0]); // close read pipeH
            close(pipeS[1]); // close write pipeS
            close(pipeM[1]); // close write pipeM
            signal(SIGUSR2, fh);
            for (;;) {
                pause(); // Chờ tín hiệu
            }
            exit(0);
        } else { // fil
            close(pipeM[0]); // close read pipeM
            close(pipeS[1]); // close write pipeS
            close(pipeH[1]); // close write pipeH
            pidH = p;
            signal(SIGUSR1, fm);
            for (;;) {
                pause(); // Chờ tín hiệu
            }
            exit(0);
        }
    } else {
        // pere
        int x;
        printf("Entrer nombre de secondes: ");
        scanf("%d",&x);

        close(pipeM[1]); // Đóng đầu ghi của pipeM
        close(pipeH[1]); // Đóng đầu ghi của pipeH

        pidM = p;
        signal(SIGALRM, fs); 
        //alarm(1);

        // flag pour verifier si dans pipe il y a quelque chose
        int flag1 = fcntl(pipeM[0], F_GETFL, 0);
        fcntl(pipeM[0], F_SETFL, flag1 | O_NONBLOCK);
        int flag2 = fcntl(pipeH[0], F_GETFL, 0);
        fcntl(pipeH[0], F_SETFL, flag2 | O_NONBLOCK);

        int sec,min,hour;
        min = 0;
        hour = 0;

        for (int i = 0; i <= x; i++) { 
            alarm(1);
            if (read(pipeS[0], &sec, sizeof(sec)) == -1) {
                perror("Error reading from pipeS");
                exit(1);
            }

            if (read(pipeM[0], &min, sizeof(min)) == -1 && errno != EAGAIN) {
                perror("Error reading from pipeM");
                exit(1);
            }
            
            if (read(pipeM[0], &min, sizeof(min)) == -1 && errno != EAGAIN) {
                perror("Error reading from pipeM");
                exit(1);
            }

            affiche(sec,min,hour);
            pause(); 
        }
        exit(0);
    }

    return 0;
}