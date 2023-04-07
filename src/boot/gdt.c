#include "gdt.h"

#define GDT_MAKE_ENTRY(base, limit, access, flags) { \
        (uint8_t)(limit >> 0), \
        (uint8_t)(limit >> 8), \
        (uint8_t)(base >> 0), \
        (uint8_t)(base >> 8), \
        (uint8_t)(base >> 16), \
        (uint8_t)(access), \
        (uint8_t)(((limit >> 16) & 0x0F) | ((flags << 4) & 0xF0)), \
        (uint8_t)(base >> 24) \
    }

GDT_ENTRY GDT[7] = {
    // null descriptor
    GDT_MAKE_ENTRY(0x00000000, 0x00000000, 0x00, 0x0),

    // 16 bit code descriptor
    GDT_MAKE_ENTRY(0x00000000, 0x0000FFFF, 0x9A, 0x8),

    // 16 bit data descriptor
    GDT_MAKE_ENTRY(0x00000000, 0x0000FFFF, 0x92, 0x8),

    // identity mapping (code, ring 0)
    GDT_MAKE_ENTRY(0x00000000, 0x000FFFFF, 0x9A, 0xC),

    // identity mapping (data, ring 0)
    GDT_MAKE_ENTRY(0x00000000, 0x000FFFFF, 0x92, 0xC),

    // identity mapping (code, ring 3)
    GDT_MAKE_ENTRY(0x00000000, 0x000FFFFF, 0xFA, 0xC),

    // identity mapping (data, ring 3)
    GDT_MAKE_ENTRY(0x00000000, 0x000FFFFF, 0xF2, 0xC)
};
