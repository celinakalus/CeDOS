#include "cedos.h"
#include "stdio.h"

#include <stdint.h>

void main(char *args) {
    int mode = 0;
    int i = 0;

    while (args[i] != 0) {
        uint8_t c = (uint8_t)(args[i]);
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

    printf("Setting graphics mode %i...\n", mode);

    graphics_set_mode(mode);

    for (int i = 0; i < 320 * 240; i++) {
        uint8_t *vmem = (uint8_t*)(0xA0000);
        vmem[i] = (uint8_t)(i);
    }
}