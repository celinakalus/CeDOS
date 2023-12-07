#include "file.h"
#include "drivers/tty.h"

#include "drivers/console.h"
#include "drivers/keyboard.h"

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
    static int state = 0;
    static char next[2];
    while (i < size) {
        if (state > 0) {
            buffer[i++] = next[--state];
            continue;
        }
        char table[] = { 
            '^', 0x1B, '1', '2', '3', '4', '5', '6', '7', '8', '9', '0', '\\', '`', 0x08, 
            '\t', 'q', 'w', 'e', 'r', 't', 'z', 'u', 'i', 'o', 'p', '?', '+', '\n',
            0, 'a', 's', 'd', 'f', 'g', 'h', 'j', 'k', 'l', '?', '?', '#',
            0, '<', 'y', 'x', 'c', 'v', 'b', 'n', 'm', ',', '.', '-', 0, 0, 0, ' ',
            0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 'A' };
        int scancode = std_kb->read();
        if (scancode < 0) {
            break;
        } else if (scancode <= 60) {
            char c = scancode <= 60 ? table[scancode] : 0;
            if (c == 0) { continue; }
            buffer[i++] = c;
        } else if (scancode == 0xE0) {
            scancode = std_kb->read();
            if (scancode > 0x48 || table[scancode] == 0) { continue; }
            buffer[i++] = '^';
            next[1] = '[';
            next[0] = table[scancode];
            state = 2;
        }
    }
    return i;
}

int tty_write(int fd, char *buffer, uint32_t size) {
    std_con->write_n(buffer, size);
    return size;
}