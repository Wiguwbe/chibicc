#include "test.h"

static int add(int a, int b) {
  return a+b;
}

// use `-unittest`
unittest {
  ASSERT(6, add(2,4));
  ASSERT(8, add(-1,9));
  printf("OK\n");
}

// use `-genmain`
