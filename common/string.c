#include "string.h"
#include <stdint.h>

void *memcpy (void *destination, const void *source, size_t num) {
    if (destination >= source) {
        for (uint32_t i = 0; i < num; i++) {
            ((uint8_t*)destination)[num - 1 - i] = ((uint8_t*)source)[num - 1 - i];
        }
    } else {
        for (uint32_t i = 0; i < num; i++) {
            ((uint8_t*)destination)[i] = ((uint8_t*)source)[i];
        }
    }
    return destination;
}

char *strcpy(char *destination, const char *source) {
    int i = 0;
    while (source[i]) {
        destination[i] = source[i];
        i++;
    }
    destination[i] = 0;
    return destination;
}

void *memset (void *ptr, int value, size_t num) {
    for (uint32_t i = 0; i < num; i++) {
        ((uint8_t*)ptr)[i] = (uint8_t)value;
    }
    return ptr;
}

unsigned int strlen (const char *str) {
    int i = 0;
    while (str[i]) { i++; }
    return i;
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

const char * strchr ( const char * str, int character ) {
    int i = 0;

    while (1) {
        if (str[i] == 0) {
            return NULL;
        }

        if (str[i] == character) {
            return &(str[i]);
        }

        i++;
    }
}