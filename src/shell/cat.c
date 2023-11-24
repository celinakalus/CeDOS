#include "cedos.h"
#include "stdio.h"

#include <stdint.h>

#define BUFFER_SIZE 1024

void main(char *args) {
    FILE* file = fopen(args, "r");

    if (file == NULL) {
        printf("Could not find file: %s\n", args);
        return;
    }

    char buffer[BUFFER_SIZE];

    while (1) {
        int size = fread(buffer, 1, 1024, file);
        if (size == 0) { break; }
        fwrite(buffer, 1, size, stdout);
    }
}