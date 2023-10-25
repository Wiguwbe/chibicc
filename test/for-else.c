#include "test.h"

int for_else(void)
{
  for(int i=0;i<1;i++)
  {
    ;  // no-op
  }
  else
  {
    // exited gracefully, should run this
    return 1;
  }
  // shouldn't reach this
  return 0;
}

int for_else_break(void)
{
  for(int i=0;i<1;i++)
  {
    break;
  }
  else
  {
    // shouldn't run this
    return 0;
  }
  // should reach this
  return 1;
}

int while_else(void)
{
  int a = 0;
  while(a < 1)
  {
    a ++;
  }
  else
  {
    // exited gracefully, should run this
    return 1;
  }
  // shouldn't reach this
  return 0;
}

int while_else_break(void)
{
  int a = 0;
  while(a < 1)
  {
    break;
  }
  else
  {
    // shouldn't run this
    return 0;
  }
  // should reach this
  return 1;
}

int main() {
  ASSERT(1, for_else());
  ASSERT(1, for_else_break());
  ASSERT(1, while_else());
  ASSERT(1, while_else_break());

  printf("OK\n");
  return 0;
}
