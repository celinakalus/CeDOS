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

    uint32_t line = 0;
    while (1) {
        int size = fread(in_buffer, 1, 16, file);
        if (size == 0) { break; }

        int out_offset = 0;

        out_offset += sprintf(out_buffer + out_offset, "%x%x  ", line++, 0);
        for (int i = 0; i < 16; i++) {
            out_offset += sprintf(out_buffer + out_offset, "%x ", in_buffer[i]);
        }
        out_offset += sprintf(out_buffer + out_offset, " |");
        for (int i = 0; i < 16; i++) {
            uint8_t c = in_buffer[i];
            if (c < 0x20 || c > 0x7F) { c = '.'; }
            out_offset += sprintf(out_buffer + out_offset, "%c", c);
        }
        out_offset += sprintf(out_buffer + out_offset, "|\n");
        fwrite(out_buffer, 1, out_offset, stdout);
    }
}