#ifndef FILE_H
#define FILE_H

#include <stdint.h>

#include "mm/paging.h"

typedef uint32_t fpos_t;
typedef int32_t off_t;

#define SEEK_SET 0
#define SEEK_CUR 1
#define SEEK_END 2

struct file;
struct file_operations;

typedef struct file file_t;
typedef struct file_operations file_operations_t;

struct file {
    file_operations_t *fops;
    uint32_t stdio_id;
    uint32_t fat_cluster;
    fpos_t pos;
    size_t size;
};

struct file_operations {
    int (*open)(const char *pathname, int flags);
    int (*openat)(file_t *root, file_t *handle, const char *fname, int flags);
    int (*read)(file_t *file, char *buffer, uint32_t size);
    int (*write)(file_t *file, char *buffer, uint32_t size);
    int (*dir_next)(file_t *file, int index, char *fname_buffer);
    off_t (*lseek)(file_t *file, off_t offset, int whence);
    off_t (*tell)(file_t *file);
};

int file_init();
int file_open(const char *pathname, int flags);
int file_openat(int fd, const char *fname, int flags);
int file_read(int fd, char *buffer, uint32_t size);
off_t file_lseek(int fd, off_t offset, int whence);
off_t file_tell(int fd);
int file_write(int fd, char *buffer, uint32_t size);
int file_dir_next(int fd, int index, char *fname_buffer);

#endif