#include "common/cedos.h"
#include "common/stdio.h"

#include <stdint.h>

void read_line(char *buffer) {
    int i = 0;
    char c;
    while (1) {
        sc_file_read(1, &c, 1);

        if (c == '\n') { break; }

        buffer[i++] = c;
        sc_file_write(0, &c, 1);
    }

    buffer[i] = 0;
    sc_file_write(0, &c, 1);
}

void main(void) {
    uint32_t a = 0, b = 1, i = 0;
    printf("\n");
    printf("CeDOS shell\n");

    while (1) {
        printf("/> ");

        char buffer[256];
        read_line(buffer);
    }
}