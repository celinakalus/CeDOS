#include "assembly.h"

#include <stdint.h>

#include "memory.h"

extern void main(char *args);

int _start(char *args) {
    malloc_init(0x20000000, 0x30000000);
    
    main(args);
}