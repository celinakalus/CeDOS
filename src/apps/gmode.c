#include "common/cedos.h"
#include "common/stdio.h"

#include <stdint.h>

void main(char *args) {
    int mode = 0;
    int i = 0;

    while (args[i] != 0) {
        char c = args[i];
        if (c >= 'a' && c <= 'f') {
            c -= 'a';
        } else if (c >= 'A' && c <= 'F') {
            c -= 'A';
        } else if (c >= '0' && c <= '9') {
            c -= '0';
        } else {
            printf("Character not recognized: %c\n", c);
        }

        mode <<= 4;
        mode += (int)(c);
        i++;
    }

    printf("Setting graphics mode %x...\n", mode);
    graphics_set_mode(mode);
}