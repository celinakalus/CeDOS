#ifndef STRING_H
#define STRING_H

#include <stdint.h>

typedef uint32_t size_t;

void *memset(void *str, int c, size_t n);

char * strcpy ( char * destination, const char * source );

#endif