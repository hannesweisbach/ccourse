#include <stdio.h>

void helper_function(int * a) {
  printf("%s(%d): a is %d\n", __func__, __LINE__, *a);
  *a += 5;
  printf("%s(%d): a is %d\n", __func__, __LINE__, *a);
}

int main() {
  int a = 5;
  printf("%s(%d): a is %d\n", __func__, __LINE__, a);
  helper_function(&a);
  printf("%s(%d): a is %d\n", __func__, __LINE__, a);
}
