#ifndef FILE_H
#define FILE_H

#include <stdint.h>

#include "cedos/mm/paging.h"

int file_read(int fd, char *buffer, uint32_t size);
int file_write(int fd, char *buffer, uint32_t size);

#endif