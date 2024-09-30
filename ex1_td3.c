#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>
#include <signal.h>
#include<string.h>

int T[NSIG]; //NSIG được định nghĩa sẵn thì phải, tùy theo phiên bản nhưng thường là 64?

void f(int s){
    T[s]++;
    for(int i=0; i< NSIG; i++){
        if(T[i]>0){
            printf("Signal %d received %d times\n", i, T[i]);
        }
    }
}

int main(){
    int i;
    for(i=0; i< NSIG; i++){
        T[i]=0;
    }
    for(i=0; i< NSIG; i++){
        signal(i,f);
    }

    for(;;); // boucle infini
    return 0;
}