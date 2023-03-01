#include "cedos.h"

#include <stdint.h>

void fib(void) {
    uint32_t a = 0, b = 1, i = 0;
    while (1) {
        uint32_t tmp = a + b;
        a = b;
        b = tmp;
        sysprint("fib (%i) = %i\n", i, a);
        i++;
        hlt();//yield();
    }
}