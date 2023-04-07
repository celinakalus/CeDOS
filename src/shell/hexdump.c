#include "cedos.h"
#include "stdio.h"

#include <stdint.h>

int hexdump(char *address, int length) {
    unsigned char *first_line = (char *)((long)address & (long)0xFFFFFFF0);
    unsigned char *last_line = (char *)((long)(address + length) & (long)0xFFFFFFF0);

    while (1) {
        if (first_line >= last_line) { break; }
        printf("%p  ", first_line);
        for (int i = 0; i < 16; i++) {
            printf("%X ", (unsigned int)(first_line[i]));
        }
        printf(" |");
        for (int i = 0; i < 16; i++) {
            uint8_t c = *(uint8_t*)(&first_line[i]);
            if (c < 0x20 || c > 0x7F) { c = '.'; }
            printf("%c", c);
        }
        printf("\n");


        first_line += 0x10;
    }
}

void main(char *args) {
    int fd = sc_file_open(args, 0);

    if (fd < 0) {
        printf("Could not find file: %s\n", args);
        return;
    }

    char *buffer = (void*)(0x2000000);

    int size = sc_file_read(fd, buffer, -1);

    hexdump(buffer, size);
}