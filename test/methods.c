
#include "test.h"

struct foo {
  int a;
  int b;
};

int foo.sum(*f) {
  return f->a + f->b;
}

struct foo foo.mul_point(f, int mult) {
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
