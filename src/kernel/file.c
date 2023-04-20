#include "cedos/file.h"
#include "cedos/fat.h"

#include "cedos/drivers/console.h"
#include "cedos/drivers/keyboard.h"

#ifdef DEBUG
#define PRINT_DBG(...) printk("[" __FILE__ "] " __VA_ARGS__)
#else
#define PRINT_DBG(...) {}
#endif

//const int root_fd = 0x1000;

FILE file_table[256];
int next_free = 0;

int stdin, stdout, fat_root;

int file_init() {
    file_table[next_free].type = FILE_STDIO;
    file_table[next_free].stdio_id = 0;
    file_table[next_free].fat_cluster = 0;
    stdin = next_free++;

    file_table[next_free].type = FILE_STDIO;
    file_table[next_free].stdio_id = 1;
    file_table[next_free].fat_cluster = 0;
    stdout = next_free++;

    // FAT root
    file_table[next_free].type = FILE_FAT;
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

    FILE *root = &file_table[fd];
    FILE *handle = &file_table[new_fd];

    if (root->type == FILE_FAT) {
        PRINT_DBG("fd is FAT\n");
        FAT_openat(root, handle, fname, flags);
        return new_fd;
    } else {
        PRINT_DBG("fd is NOT FAT (%i)\n", (int)(root->type));
        // open other files
        return 0;
    }
}

int file_read(int fd, char *buffer, uint32_t size) {
    FILE *file = &file_table[fd];

    if (file->type == FILE_FAT) {
        return FAT_read(file, buffer, size);
    } else {
        uint32_t i = 0;
        while (i < size) {
            char table[] = { 
                '^', 0x1B, '1', '2', '3', '4', '5', '6', '7', '8', '9', '0', '\\', '`', 0x08, 
                '\t', 'q', 'w', 'e', 'r', 't', 'z', 'u', 'i', 'o', 'p', '?', '+', '\n',
                0, 'a', 's', 'd', 'f', 'g', 'h', 'j', 'k', 'l', '?', '?', '#',
                0, '<', 'y', 'x', 'c', 'v', 'b', 'n', 'm', ',', '.', '-', 0, 0, 0, ' ' };
            uint32_t scancode = std_kb->read();
            char c = scancode <= 60 ? table[scancode] : 0;
            if (c == 0) { continue; }
            buffer[i++] = c;
        }
        return size;
    }
}

int file_write(int fd, char *buffer, uint32_t size) {
    for (uint32_t i = 0; i < size; i++) {
        std_con->write_c(buffer[i]);
    }
    return size;
}

int file_dir_next(int fd, int index, char *fname_buffer) {
    if (fd & 0x1000) {
        return FAT_dir_next(fd, index, fname_buffer);
    } else {
        return -1;
    }
}
