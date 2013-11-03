#include <stdio.h>
#include <stdlib.h>

int main(int argc, char *argv[]) {
  int a;
  int b;
  int sum = 0;

  if (argc < 3) {
    printf("Missing argument(s). Usage: %s <op1> <op2>\n", argv[0]);
    return EXIT_FAILURE;
  }

  sscanf(argv[1], "%d", &a);
  sscanf(argv[2], "%d", &b);

  // sum???

  printf("%s + %s = %d\n", argv[1], argv[2], sum);

  return EXIT_SUCCESS;
}
