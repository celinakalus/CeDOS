#include "cedos.h"
#include "stdio.h"

#include <stdint.h>

void main(char *args) {
    uint32_t memusage = sc_mem_usage();
    printf("%i KB\n", memusage / 1000);
}