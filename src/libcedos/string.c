#include "string.h"
#include <stdint.h>

void *memset(void *str, int c, size_t n) {
    uint8_t *dest = str;
    for (int i = 0; i < n; i++) {
        dest[i] = c;
    }
}

char * strcpy ( char * destination, const char * source ) {
    int i = 0;

    while (source[i]) {
        destination[i] = source[i];
        i++;
    }
}