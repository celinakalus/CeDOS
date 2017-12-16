#include "gdt.h"

#define GDT_MAKE_ENTRY(base, limit, access, flags) { \
        (uint8_t)(limit >> 0), \
        (uint8_t)(limit >> 8), \
        (uint8_t)(base >> 0), \
        (uint8_t)(base >> 8), \
        (uint8_t)(base >> 16), \
        (uint8_t)(access), \
        (uint8_t)(((limit >> 16) & 0x0F) | ((flags << 4) & 0xF0)), \
        (uint8_t)(base >> 24), \
    }

GDT_ENTRY GDT[3] = {
    // null descriptor
    GDT_MAKE_ENTRY(0x00000000, 0x00000000, 0x00, 0x0),

    // identity mapping (code)
    GDT_MAKE_ENTRY(0x00000000, 0x000FFFFF, 0x9A, 0xC),

    // identity mapping (data)
    GDT_MAKE_ENTRY(0x00000000, 0x000FFFFF, 0x92, 0xC)
};