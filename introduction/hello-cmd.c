#include <stdio.h>
#include <stdlib.h>

int main(int argc, char *argv[]) {
  
  if (argc < 2) {
    printf("You didn't tell me your name :(\n");
    printf("Use the program like this: %s <your-name>\n", argv[0]);
    return EXIT_FAILURE;
  }

  printf("Hello ???\n");

  return EXIT_SUCCESS;
}
