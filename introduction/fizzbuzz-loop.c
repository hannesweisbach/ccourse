#include <stdio.h>
#include <stdlib.h>

void fizzbuzz(int fizzbuzz) {
    if(fizzbuzz % 3 == 0)
        printf("Fizz");
    if(fizzbuzz % 5 == 0)
        printf("Buzz");
    if((fizzbuzz % 3 != 0) && (fizzbuzz % 5 != 0))
        printf("%d", fizzbuzz);
     printf("\n");
}

int main() {
    for(int i = 0; i <= 100; i++) {
        fizzbuzz(i);
    }
    
    return EXIT_SUCCESS;
}
