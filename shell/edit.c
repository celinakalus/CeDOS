#include "cedos.h"
#include "stdio.h"
#include "string.h"
#include "stdlib.h"

#include <stdint.h>

#define BUFFER_SIZE 250

struct linebuf {
    int length;

    char buffer[BUFFER_SIZE];

    struct linebuf *next;
};

struct linebuf *first;

void main(char *args) {
    FILE* file = fopen(args, "r");

    if (file == -1) {
        printf("Could not find file: %s\n", args);
        return;
    }

    int line_num = 0;

    printf("\e[1;1H");

    while (1) {
        struct linebuf *next = malloc(sizeof(struct linebuf));
        memset(next->buffer, 0, BUFFER_SIZE);

        int file_pos = ftell(file);
        int num_read = fread(next->buffer, BUFFER_SIZE - 1, sizeof(char), file);

        if (num_read == 0) {
            break;
        }

        char *newline = strchr(next->buffer, '\n');
        *newline = 0;
        next->length = newline - next->buffer;
        file_pos = file_pos + next->length + 1;

        printf("\e[93m%i\e[0K\e[3G| \e[0m%s\n", line_num++, next->buffer);

        if (line_num > 23) { break; }

        // rewind until after the newline
        fseek(file, file_pos, SEEK_SET);
        
        if (file_pos != ftell(file)) {
            break;
        }
    }
}