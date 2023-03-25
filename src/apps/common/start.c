#include "assembly.h"

#include <stdint.h>

extern void main(char *args);

int _start(char *args) {
    main(args);
}