
#include "test.h"

#template "template.tpl" key=value integer=6 default=n value=70

int main() {
  ASSERT(0, strcmp(key, "value"));

  ASSERT(6, integer);

  ASSERT(70, default_value);

  return 0;
}
