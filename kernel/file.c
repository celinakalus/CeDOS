#include "file.h"
#include "fat.h"
#include "pipe.h"

#include "drivers/tty.h"
#include "core.h"

#include "memory.h"

#include "sched/sched.h"
#include "sched/process.h"

#ifdef DEBUG
#define PRINT_DBG(...) printk("[" __FILE__ "] " __VA_ARGS__)
#else
#define PRINT_DBG(...) {}
#endif

//const int root_fd = 0x1000;

file_t *file_table;
int next_free = 0;

int stdin, stdout, fat_root, pipe;

int file_init() {
    file_table = malloc(sizeof(file_t) * 512);

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

    // pipe
    file_table[next_free].fops = &pipe_fops;
    pipe = next_free++;

    return 0;
}

int file_open(const char *pathname, int flags) {
    while (*pathname == '/') {
        pathname++;
    }

    return file_openat(fat_root, pathname, flags);
}

file_t *get_process_local_file(int fd) {
    if (fd > 1) { return &file_table[fd]; }
    
    PROCESS_ID pid = get_current_process();
    PRINT_DBG("pid: %i\n", pid);
    PROCESS *p = get_process(pid);

    if (fd == 0) { return &file_table[p->stdin]; }
    if (fd == 1) { return &file_table[p->stdout]; }

    return NULL;
}

int file_openat(int fd, const char *fname, int flags) {
    int new_fd = next_free++;

    PRINT_DBG("Given fd: %i\n", fd);
    PRINT_DBG("New fd:   %i\n", new_fd);

    file_t *root = get_process_local_file(fd);
    file_t *handle = &file_table[new_fd];

    if (fd < 0) { return -1; }

    if (root->fops->openat == NULL) { return -1; }

    if (root->fops->openat(root, handle, fname, flags)) { return -1; }

    return new_fd;
}

int file_read(int fd, char *buffer, uint32_t size) {
    if (fd < 0) { return 0; }

    file_t *file = get_process_local_file(fd);

    if (file->fops->read == NULL) { return -1; }

    return file->fops->read(file, buffer, size);
}

int file_write(int fd, char *buffer, uint32_t size) {
    if (fd < 0) { return 0; }

    file_t *file = get_process_local_file(fd);

    if (file->fops->write == NULL) { return -1; }

    file->fops->write(file, buffer, size);
}

int file_dir_next(int fd, int index, char *fname_buffer) {
    if (fd < 0) { return -1; }

    file_t *file = get_process_local_file(fd);

    if (file->fops->dir_next == NULL) { return -1; }

    file->fops->dir_next(file, index, fname_buffer);
}

off_t file_lseek(int fd, off_t offset, int whence) {
    if (fd < 0) { return -1; }

    file_t *file = get_process_local_file(fd);

    if (file->fops->lseek == NULL) { return -1; }

    return file->fops->lseek(file, offset, whence);
}

off_t file_tell(int fd) {
    if (fd < 0) { return -1; }

    file_t *file = get_process_local_file(fd);

    if (file->fops->tell == NULL) { return -1; }

    return file->fops->tell(file);
}