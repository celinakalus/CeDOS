#include "cedos/file.h"

#include "cedos/drivers/console.h"
#include "cedos/drivers/keyboard.h"

int file_read(int fd, char *buffer, uint32_t size) {
    for (int i = 0; i < size; i++) {
        char table[] = { 
            0, 0, '1', '2', '3', '4', '5', '6', '7', '8', '9', '0', '\\', '´', 0x08, 
            '\t', 'q', 'w', 'e', 'r', 't', 'z', 'u', 'i', 'o', 'p', 'ü', '+', '\n',
            0, 'a', 's', 'd', 'f', 'g', 'h', 'j', 'k', 'l', 'ö', 'ä', '#',
            0, '<', 'y', 'x', 'c', 'v', 'b', 'n', 'm', ',', '.', '-'};
        uint32_t scancode = std_kb->read();
        char c = scancode >= 0 && scancode <= 60 ? table[scancode] : 0;
        buffer[i] = c;
    }
}

int file_write(int fd, char *buffer, uint32_t size) {
    for (int i = 0; i < size; i++) {
        std_con->write_c(buffer[i]);
    }
}