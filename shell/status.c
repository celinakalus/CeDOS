#include "cedos.h"
#include "stdio.h"
#include "string.h"

#include <stdint.h>

void main(char *args) {
    char buffer[81];

    while (1) {
        memset(buffer, ' ', sizeof(buffer) - 1);

        datetime_t now;
        time_now(&now);

        sprintf(buffer + 60, "%i-%i-%i %i:%i:%i", now.year, now.month, now.day, now.hour, now.minute, now.second);
        printf("\e[s\e[0;0H\e[41m%s\e[u\e[0m", buffer);

        yield();
    }
}