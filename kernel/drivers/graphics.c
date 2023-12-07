#include "drivers/graphics.h"
#include "mm/paging.h"

#include "string.h"
#include "linker.h"

#include <stdint.h>

extern void realmode_int10h(uint32_t eax, uint32_t ebx, uint32_t ecx);

int graphics_init(void) {
    VIRT_ADDR src = (VIRT_ADDR)((uint32_t)(KERNEL_VMA) + (uint32_t)(REALMD_LMA));
    uint32_t size = REALMD_SIZE;
    VIRT_ADDR dest = (VIRT_ADDR)(REALMD_VMA);

    memcpy(dest, src, size);
    return 0;
}

int graphics_set_mode(int mode) {
    realmode_int10h((uint32_t)(mode), 0, 0);
    return 0;
}