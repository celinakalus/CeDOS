#include "cedos.h"
#include "stdio.h"

#include <stdint.h>


void main(char *args) {
    FILE* file = fopen(args, "r");

    if (file == NULL) {
        printf("Could not find file: %s\n", args);
        return;
    }

    uint8_t in_buffer[16];
    uint8_t out_buffer[64];

    while (1) {
        int in_offset = ftell(file);

        int size = fread(in_buffer, 1, 16, file);
        if (size == 0) { break; }

        int out_offset = 0;
        out_offset += sprintf(out_buffer + out_offset, "%x%x%x%x  ", in_offset >> 24, in_offset >> 16, in_offset >> 8, in_offset);
        for (int i = 0; i < 16; i++) {
            if (i < size) {
                out_offset += sprintf(out_buffer + out_offset, "%x ", in_buffer[i]);
            } else {
                out_offset += sprintf(out_buffer + out_offset, "   ");
            }
        }
        out_offset += sprintf(out_buffer + out_offset, " |");
        for (int i = 0; i < 16; i++) {
            if (i >= size) { break; }
            uint8_t c = in_buffer[i];
            if (c < 0x20 || c > 0x7F) { c = '.'; }
            out_offset += sprintf(out_buffer + out_offset, "%c", c);
        }
        out_offset += sprintf(out_buffer + out_offset, "|\n");
        fwrite(out_buffer, 1, out_offset, stdout);
    }
}