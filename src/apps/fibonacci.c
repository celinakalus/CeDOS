#include "cedos.h"

#include <stdint.h>

void fib(void) {
    uint32_t a = 0, b = 1, i = 0;
    while (1) {
        uint32_t tmp = a + b;
        a = b;
        b = tmp;
        printf("fib (%i) = %i\n", i, a);
        i++;
        char c = 0;
        sc_file_read(0, &c, 1);
        printf("[%c]\n", (int)(uint32_t)c);//yield();
    }
}