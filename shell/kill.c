#include "cedos.h"

#include <stdint.h>
#include "stdlib.h"
#include "string.h"

#include "cedos.h"

void main(char *args) {
    int pid = atoi(args);

    process_kill(pid);
}