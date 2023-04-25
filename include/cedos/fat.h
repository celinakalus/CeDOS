#ifndef FAT_H
#define FAT_H

#include <stdint.h>

#include "cedos/file.h"

void FAT_init();
int FAT_dir_next(int fd, int index, char *fname_buffer);
int FAT_openat(FILE *root, FILE *handle, const char *fname, int flags);
uint32_t FAT_read(FILE *file, void *buffer, int count);


#endif