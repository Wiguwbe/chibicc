#include "test.h"

static int global_var = 8;

void __attribute__((constructor)) _constructor(void) {
  global_var = 17;
}

// there's no easy way to test this
void __attribute__((destructor)) _destructor(void) {
  global_var = 8;
}

int main() {

  ASSERT(17, global_var);

  printf("OK\n");
  return 0;
}
