#include <stdio.h>
#include <stdlib.h>

int main(int argc, char const *argv[]){
    int n = 0;
    if (argc == 2){
        n = atoi(argv[1]) * 2;
    }
    
    for (int i = 2; i <= n; i+=2){
        printf("%d\n", i);
    }
    

    return 0;
}