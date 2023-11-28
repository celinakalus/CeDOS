#ifndef STRING_H
#define STRING_H

#include <stdint.h>

#define NULL (void*)(0)

void *memset(void *ptr, uint8_t value, uint32_t num);
int strcmp(const char *str1, const char *str2);
void *memcpy(void *dest, const void * src, uint32_t n);

#endif