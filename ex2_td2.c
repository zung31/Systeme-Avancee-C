#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>
#define Max 64 

char majToMin(char c){
    if(c>='A' && c<='Z'){
        c +=32; // nếu không nhớ thì lấy (a-A) nhưng nhớ là A sẽ nhỏ hơn a 
    }
    return c;
}

int main() {
    pid_t p;
    int s, n, T[2];
    char temps[Max];
    if(pipe(T)==-1){
        perror("error pipe");
        exit(1);
    }
    p = fork();
    if(p<0){
        perror("Fil n'est pas crée");
        exit(1);
    }
    if(p == 0){
        close(T[0]);
        while(n = read(0, temps, Max)){ // nếu người dùng vẫn muốn nhập thì n sẽ != 0
            // read trả về số byte đã đọc từ file desxriptor -> ở trg hợp này thì sẽ trả về longeur của nhỏ chuỗi vừa nhập
            write(T[1], temps, n); //ghi chuỗi temps vào tube
        }
    }
    else{//pere
        close(T[1]);// đóng đầu ghi nếu ko định gửi j đến con
        while(n = read(T[0], temps, Max)){ // n lúc này chưa có valeur nên phải dùng Max và n được gán valeur
            for(int i=0; i<n; i++){ // sau mới dc dùng n nè =))
                temps[i] = majToMin(temps[i]);
            }
            write(1, temps, n); // in ra màn hình
        }
        close(T[0]);
        wait(&s);
    }

    return 0;
}