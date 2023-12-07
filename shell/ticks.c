#include "cedos.h"
#include "stdio.h"

#include <stdint.h>
#include "stdlib.h"
#include "string.h"

#include "cedos.h"

void main(char *args) {
    if (args == NULL || strlen(args) == 0) {
        int ticks = sc_time_get_ticks();
        printf("%i\n", ticks);
    } else {
        int interval = atoi(args);

        if (interval <= 0) {
            printf("Invalid interval value.\n");
            return;
        }

        int pid = get_pid();
        int color = 40 + (pid % 8);

        int superticks = 0;

        while (1) {
            int ticks = sc_time_get_ticks();

            if (ticks / interval != superticks) {
                printf("\e[%im\e[0K[%i] %i ticks\e[0m\n", color, pid, ticks);
                superticks = ticks / interval;
            }

            sleep(interval);
        }
    }
}