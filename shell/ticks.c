#include "cedos.h"
#include "stdio.h"

#include <stdint.h>

void main(char *args) {
    int ticks = sc_time_get_ticks();
    printf("%i\n", ticks);
}