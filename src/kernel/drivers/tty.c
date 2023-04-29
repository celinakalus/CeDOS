#include "cedos/file.h"
#include "cedos/drivers/tty.h"

#include "cedos/drivers/console.h"
#include "cedos/drivers/keyboard.h"

file_operations_t tty_fops = {
    tty_open,       /* open */
    tty_openat,     /* openat */
    tty_read,       /* read */
    tty_write,      /* write */
    NULL,           /* dir_next */
    NULL            /* lseek */
}; 

int tty_open(const char *pathname, int flags) {

}

int tty_openat(int fd, const char *fname, int flags) {

}

int tty_read(int fd, char *buffer, uint32_t size) {
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

int tty_write(int fd, char *buffer, uint32_t size) {
    for (uint32_t i = 0; i < size; i++) {
        std_con->write_c(buffer[i]);
    }
    return size;
}