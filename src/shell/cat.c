#include "cedos.h"
#include "stdio.h"

#include <stdint.h>

void main(char *args) {
    int fd = sc_file_open(args, 0);

    if (fd < 0) {
        printf("Could not find file: %s\n", args);
        return;
    }

    char *buffer = (void*)(0x2000000);

    int size = sc_file_read(fd, buffer, -1);

    while (size > 0) {
        int chunk = 256;
        if (size < chunk) { chunk = size; }
        sc_file_write(0, buffer, chunk);
        size -= chunk;
        buffer += chunk;
    }
}