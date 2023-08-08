#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <signal.h>

void ErrorHandlerHUP(int signalNum){
    printf("Ouch!\n");
}

void ErrorHandlerINT(int signalNum){
    printf("Yeah!\n");
}

// Takes a number n in the command line and prints out
// the first n even numbers
int main(int argc, char const *argv[]){
    signal(SIGINT, ErrorHandlerHUP);
    signal(SIGHUP, ErrorHandlerINT);

    int n = 0;
    if (argc == 2){
        n = atoi(argv[1]);
    }
    
    for (int i = 0; i < n; i+=2){
        printf("%d\n", i);
        sleep(5);
    }
    

    return 0;
}