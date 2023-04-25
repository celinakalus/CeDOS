#include "cedos/file.h"
#include "cedos/fat.h"

#include "cedos/drivers/tty.h"
#include "cedos/core.h"

#ifdef DEBUG
#define PRINT_DBG(...) printk("[" __FILE__ "] " __VA_ARGS__)
#else
#define PRINT_DBG(...) {}
#endif

//const int root_fd = 0x1000;

file_t file_table[256];
int next_free = 0;

int stdin, stdout, fat_root;

int file_init() {
    file_table[next_free].fops = &tty_fops;
    file_table[next_free].stdio_id = 0;
    file_table[next_free].fat_cluster = 0;
    stdin = next_free++;

    file_table[next_free].fops = &tty_fops;
    file_table[next_free].stdio_id = 1;
    file_table[next_free].fat_cluster = 0;
    stdout = next_free++;

    // FAT root
    file_table[next_free].fops = &FAT_fops;
    file_table[next_free].stdio_id = 0;
    file_table[next_free].fat_cluster = 0;
    fat_root = next_free++;

    return 0;
}

int file_open(const char *pathname, int flags) {
    while (*pathname == '/') {
        pathname++;
    }

    return file_openat(fat_root, pathname, flags);
}

int file_openat(int fd, const char *fname, int flags) {
    int new_fd = next_free++;

    PRINT_DBG("Given fd: %i\n", fd);
    PRINT_DBG("New fd:   %i\n", new_fd);

    file_t *root = &file_table[fd];
    file_t *handle = &file_table[new_fd];

    if (root->fops->openat == NULL) { return -1; }

    if (root->fops->openat(root, handle, fname, flags)) { return -1; }

    return new_fd;
}

int file_read(int fd, char *buffer, uint32_t size) {
    file_t *file = &file_table[fd];

    if (file->fops->read == NULL) { return -1; }

    file->fops->read(file, buffer, size);
}

int file_write(int fd, char *buffer, uint32_t size) {
    file_t *file = &file_table[fd];

    if (file->fops->write == NULL) { return -1; }

    file->fops->write(file, buffer, size);
}

int file_dir_next(int fd, int index, char *fname_buffer) {
    file_t *file = &file_table[fd];

    if (file->fops->dir_next == NULL) { return -1; }

    file->fops->dir_next(file, index, fname_buffer);
}
