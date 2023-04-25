#ifndef FAT_H
#define FAT_H

#include <stdint.h>

#include "cedos/file.h"

void FAT_init();
int FAT_dir_next(file_t *file, int index, char *fname_buffer);
int FAT_openat(file_t *root, file_t *handle, const char *fname, int flags);
uint32_t FAT_read(file_t *file, void *buffer, int count);

extern file_operations_t FAT_fops;


#endif