#include "cedos/drivers/graphics.h"

#include <stdint.h>

extern void realmode_int10h(uint32_t eax, uint32_t ebx, uint32_t ecx);

int graphics_set_mode(int mode) {
    realmode_int10h((uint32_t)(mode), 0, 0);
}