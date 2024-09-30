#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/wait.h>
#include <unistd.h>

volatile sig_atomic_t s = 0, m = 0, h = 0; // Biến giờ, phút, giây
volatile sig_atomic_t pidM, pidH; // PID của các tiến trình phút và giờ

void affiche(int time) {
  printf("%d\n", time); // In giá trị hiện tại
  fflush(stdout); // Đảm bảo rằng giá trị được in ra ngay lập tức
}

// signum nhận giá trị là số hiệu của tín hiệu mà tiến trình nhận được như sigalrm, sigint,sigusr1,..
void fs(int signum) {
    affiche(s);
    s++;
    if (s == 60) {
        s = 0;
        kill(pidM, SIGUSR1);  // Gửi tín hiệu cho quá trình M -> gửi SIGUSR1 cho quá trình pidM
    }
    alarm(1);
}

void fm(int signum) {
    affiche(m);
    m++;
    if (m == 60) {
        m = 0;
        kill(pidH, SIGUSR2);
    }
}

void fh(int signum) {
    affiche(h);
    h++;
    if (h == 24) {
        h = 0;
    }
}

// fs ở cha, fm ở fil và fh ở fil của fil
int main() {
    pid_t p;
    // không nên dùng wait ở đây vì wait sẽ chặn pere cho đến khi các fils xong 
    // mà chương trình cần sự giao tiếp tín hiệu giữa cả con và cha -> bỏ wait
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
            signal(SIGUSR2, fh);
            for (;;) {
                pause(); // Chờ tín hiệu
            }
            exit(0);
        } else { // fil
            //wait(&status);
            pidH = p;
            signal(SIGUSR1, fm);
            for (;;) {
                pause(); // Chờ tín hiệu
            }
            exit(0);
        }
    } else {
        // pere
        //wait(&status);
        pidM = p;
        signal(SIGALRM, fs); // mỗi khi alarm thì lại thực hiện 
        alarm(1); // Bắt đầu chu kỳ báo thức
        for (;;) {
            pause(); // chờ tín hiệu
        }
        exit(0);
    }

    return 0;
}