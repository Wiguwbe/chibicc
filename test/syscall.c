#include "test.h"

int main() {
    // sys_exit
    _syscall(60, 0);

    // it should exit there
    assert(0, 1, "syscall");
}
