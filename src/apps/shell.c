#include "common/cedos.h"
#include "common/stdio.h"

#include <stdint.h>

void read_line(char *buffer) {
    int i = 0;
    char c;
    buffer[0] = 0;
    while (1) {
        sc_file_read(1, &c, 1);

        if (c == '\n') { break; }
        if (c == 0) { continue; }
        if (c == 0x08 && i <= 0) { continue; }

        if (c == 0x08) {
            buffer[--i] = 0;
            sc_file_write(0, &c, 1);
        } else {
            buffer[i++] = c;
            sc_file_write(0, &c, 1);
        }
    }

    buffer[i] = 0;
    sc_file_write(0, &c, 1);
}

void main(char *args) {
    uint32_t a = 0, b = 1, i = 0;
    printf("\n");
    printf("CeDOS shell\n");

    while (1) {
        printf("/> ");

        char buffer[256];
        read_line(buffer);

        //printf("Executing %s...\n", buffer);

        int pid = process_spawn(buffer, "Hello, world!\n");

        //printf("Child process %i spawned, waiting for termination...\n", pid);

        if (pid == -1) {
            printf("File not found: %s\n", buffer);
        } else {
            process_wait(pid);
        }

        //printf("Child process %i terminated.\n", pid);
    }
}