#ifndef FILE_H
#define FILE_H

#include <stdint.h>

#include "cedos/mm/paging.h"

int file_open(const char *pathname, int flags);
int file_openat(int fd, const char *fname, int flags);
int file_read(int fd, char *buffer, uint32_t size);
int file_write(int fd, char *buffer, uint32_t size);
int file_dir_next(int fd, int index, char *fname_buffer);

#endif