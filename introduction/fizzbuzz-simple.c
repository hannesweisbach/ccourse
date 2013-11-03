#include <stdio.h>
#include <stdlib.h>

int main(int argc, char * argv[]) {
    int fizzbuzz;

    if (argc < 2) {
        printf("Missing argument(s). Usage: %s <op1>\n", argv[0]);
        return EXIT_FAILURE;
    }

    sscanf(argv[1], "%d", &fizzbuzz);

    /* ???? */

    return EXIT_SUCCESS;
}
