#include "assembly.h"

#include <stdint.h>

extern void main(void);

int _start(char *args) {
    main();

    while(1);
}