#include <stdio.h>
#include <stdlib.h>

int main() {
    for(int fizzbuzz = 0; fizzbuzz <= 100; fizzbuzz++) {
        if(fizzbuzz % 3 == 0)
            printf("Fizz");
        if(fizzbuzz % 5 == 0)
            printf("Buzz");
        if((fizzbuzz % 3 != 0) && (fizzbuzz % 5 != 0))
            printf("%d", fizzbuzz);
         printf("\n");
    }
    
    return EXIT_SUCCESS;
}
