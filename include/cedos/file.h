#ifndef FILE_H
#define FILE_H

#include <stdint.h>

#include "cedos/mm/paging.h"

typedef uint32_t fpos_t;

typedef struct {
    enum {
        FILE_STDIO = 0,
        FILE_FAT = 1
    } type;
    uint32_t stdio_id;
    uint32_t fat_cluster;
    fpos_t pos;
} FILE;

int file_init();
int file_open(const char *pathname, int flags);
int file_openat(int fd, const char *fname, int flags);
int file_read(int fd, char *buffer, uint32_t size);
int file_write(int fd, char *buffer, uint32_t size);
int file_dir_next(int fd, int index, char *fname_buffer);

#endif