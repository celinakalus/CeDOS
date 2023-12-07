#include "cedos.h"
#include "stdio.h"

void assert_failed(const char * message) {
    printf(message);

    int pid = get_pid();

    /* TODO: Kill this process */
    while (1) {}
}
