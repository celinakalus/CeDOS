#include "cedos.h"
#include "stdio.h"

#include <stdint.h>

#define GMODE_TEXT 0x02
#define GMODE_VIDEO 0x13

#define MAX(x, y) (x > y ? x : y)
#define MIN(x, y) (x < y ? x : y)

typedef struct __attribute__((packed)) {
    uint16_t signature;
    uint32_t file_size;
    uint16_t reserved_1;
    uint16_t reserved_2;
    uint32_t offset_pixelarray;
} BMP_FILE_HEADER;

typedef struct {
    uint32_t DIB_header_size;
    uint32_t image_width;
    uint32_t image_height;
    /* ... */
} DIB_HEADER;

uint32_t convert_endian(uint32_t value) {
    uint32_t res = 0;

    return value;

    for (int i = 0; i < 2; i++) {
        uint8_t c = value & 0x0000FFFF;
        value = value >> 16;
        res = res << 16;
        res |= c;
    }

    return res;
}

void draw_image(uint8_t *imgbuff, int off_x, int off_y, int width, int height) {
    uint8_t *gbuff = (uint8_t*)(0xA0000);

    for (int x = 0; x < MIN(width, 320); x++) {
        for (int y = 0; y < MIN(height, 240); y++) {
            int img_i = (height - y) * width + x;
            int g_i = y * 320 + x;

            gbuff[g_i] = imgbuff[img_i];
        }
    }
}

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

    BMP_FILE_HEADER *bmp_header = (BMP_FILE_HEADER*)(fbuff);
    DIB_HEADER *dib_header = (DIB_HEADER*)(&(bmp_header[1]));

    int offset = bmp_header->offset_pixelarray;
    int width = dib_header->image_width;
    int height = dib_header->image_height;

    uint8_t *imgbuff = fbuff + offset;

    int off_x = 0;
    int off_y = 0;

    // switch video mode and display image
    graphics_set_mode(GMODE_VIDEO);

    draw_image(imgbuff, off_x, off_y, width, height);

    while (1) {
        char c;
        sc_file_read(0, &c, 1);
        if (c == 0x1B) { break; }
    }

    graphics_set_mode(GMODE_TEXT);
}