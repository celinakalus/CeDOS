#include "cedos.h"
#include "stdio.h"

#include <stdint.h>

void main(char *args) {
    char buffer[256];

    int index = 0;

    while (1) {
        int next = dir_next(2, index, buffer);

        if (next == -1) { return; }

        printf("%s\n", buffer);
        index = next;
    }
}