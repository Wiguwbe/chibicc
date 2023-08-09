
#include "test.h"

struct foo {
  int a;
  int b;
};

int (struct foo *f) sum() {
  return f->a + f->b;
}

struct foo (struct foo f) mul_point(int mult) {
  return (struct foo){ f.a * mult, f.b * mult };
}

int main() {
  struct foo t = { .a = 2, .b = 5 };

  ASSERT(7, t.sum());

  struct foo m = t.mul_point(3);

  ASSERT(6, m.a);
  ASSERT(15, m.b);

  return 0;
}
