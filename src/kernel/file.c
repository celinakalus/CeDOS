#include "cedos/file.h"

#include "cedos/drivers/console.h"
#include "cedos/drivers/keyboard.h"

const int root_fd = 0x1000;

int file_open(const char *pathname, int flags) {
    while (*pathname == '/') {
        pathname++;
    }

    return file_openat(0x1000, pathname, flags);
}

int file_openat(int fd, const char *fname, int flags) {
    if (fd & 0x1000) {
        return FAT_openat(fd, fname, flags);
    } else {
        // open other files
    }
}

int file_read(int fd, char *buffer, uint32_t size) {
    if (fd & 0x1000) {
        return FAT_read(fd, buffer, size);
    } else {
        int i = 0;
        while (i < size) {
            char table[] = { 
                '^', 0x1B, '1', '2', '3', '4', '5', '6', '7', '8', '9', '0', '\\', '´', 0x08, 
                '\t', 'q', 'w', 'e', 'r', 't', 'z', 'u', 'i', 'o', 'p', 'ü', '+', '\n',
                0, 'a', 's', 'd', 'f', 'g', 'h', 'j', 'k', 'l', 'ö', 'ä', '#',
                0, '<', 'y', 'x', 'c', 'v', 'b', 'n', 'm', ',', '.', '-', 0, 0, 0, ' ' };
            uint32_t scancode = std_kb->read();
            char c = scancode >= 0 && scancode <= 60 ? table[scancode] : 0;
            if (c == 0) { continue; }
            buffer[i++] = c;
        }
    }
}

int file_write(int fd, char *buffer, uint32_t size) {
    for (int i = 0; i < size; i++) {
        std_con->write_c(buffer[i]);
    }
}