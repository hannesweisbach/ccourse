#include <stdio.h>
#include <stdlib.h>

int main(int argc, char *argv[]) {
  int a;
  int b;
  int conversions;
  int sum = 0;

  if (argc < 3) {
    printf("Missing argument(s). Usage: %s <op1> <op2>\n", argv[0]);
    return EXIT_FAILURE;
  }

  conversions = sscanf(argv[1], "%d", &a);
  conversions += sscanf(argv[2], "%d", &b);
  if (conversions != 2) {
    printf("Could not parse input '%s' or '%s' as number\n", argv[1], argv[2]);
    return EXIT_FAILURE;
  }

  // sum???

  printf("%s + %s = %d\n", argv[1], argv[2], sum);

  return EXIT_SUCCESS;
}
