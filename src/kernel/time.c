#include "cedos/time.h"

int ticks = 0;

void time_tick(void) {
    ticks++;
}

int time_get_ticks(void) {
    return ticks;
}