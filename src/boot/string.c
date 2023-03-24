#include "string.h"

#include <stdint.h>

void *memset(void *ptr, uint8_t value, uint32_t num) {
    for (uint32_t i = 0; i < num; i++) {
        ((uint8_t*)ptr)[i] = value;
    }
    return ptr;
}

int strcmp(const char *str1, const char *str2) {
    int i = 0;

    while (1) {
        if (str1[i] != str2[i]) {
            return str1[i] - str2[i];
        }

        if (str1[i] == 0) {
            return 0;
        }

        i++;
    }
}

void *memcpy(void *_dest, const void *_src, uint32_t n) {
    uint8_t *src = (uint8_t*)(_src);
    uint8_t *dest = (uint8_t*)(_dest);

    for (int i = 0; i < n; i++) {
        dest[i] = src[i];
    }
}