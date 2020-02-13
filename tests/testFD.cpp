#include <stdio.h>
#include <fcntl.h>
#include <unistd.h>


int main(){
    int i = 0;
    while(true){
        open(i);
        printf("i: %d\n", i);

    }
}