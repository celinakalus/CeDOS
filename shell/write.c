#include "cedos.h"
#include "stdio.h"

#include <stdint.h>

void main(char *args) {
    FILE *file = fopen("test.txt", "rw");
    fseek(file, 0, SEEK_SET);
    fprintf(file, "%s\n", args);
}