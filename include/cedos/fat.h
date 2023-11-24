#ifndef FAT_H
#define FAT_H

#include <stdint.h>

#include "cedos/file.h"

void FAT_init();
int FAT_dir_next(file_t *file, int index, char *fname_buffer);
int FAT_openat(file_t *root, file_t *handle, const char *fname, int flags);
uint32_t FAT_read(file_t *file, uint8_t *buffer, uint32_t count);
off_t FAT_lseek(file_t *file, off_t offset, int whence);
off_t FAT_tell(file_t *file);

extern file_operations_t FAT_fops;


#endif