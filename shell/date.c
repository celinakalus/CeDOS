#include "cedos.h"
#include "stdio.h"

#include <stdint.h>

void main(char *args) {
    datetime_t now;
    time_now(&now);
    printf("%i-%i-%i %i:%i:%i\n", now.year, now.month, now.day, now.hour, now.minute, now.second);

}