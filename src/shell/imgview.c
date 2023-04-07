#include "cedos.h"
#include "stdio.h"

#include <stdint.h>

#define GMODE_TEXT 0x02
#define GMODE_VIDEO 0x13

#define MAX(x, y) (x > y ? x : y)

void main(char *args) {
    // open image file
    int fd = sc_file_open(args, 0);

    if (fd < 0) {
        printf("Could not find file: %s\n", args);
        return;
    }

    uint8_t *fbuff = (uint8_t*)(0x2000000);

    int size = sc_file_read(fd, fbuff, -1);
    int limit = 320 * 240;

    uint8_t *gbuff = (uint8_t*)(0xA0000);

    // switch video mode and display image
    graphics_set_mode(GMODE_VIDEO);

    for (int i = 0; i < MAX(size, limit); i++) {
        gbuff[i] = fbuff[i];
    }

    while (1) {
        char c;
        sc_file_read(0, &c, 1);
        if (c == 0x1B) { break; }
    }

    graphics_set_mode(GMODE_TEXT);
}