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

void main(char *args) {
    // open image file
    int fd = fopen(args, "r");

    if (fd < 0) {
        printf("Could not find file: %s\n", args);
        return;
    }

    int limit = 320 * 240;

    BMP_FILE_HEADER bmp_header;
    DIB_HEADER dib_header;

    int size = 0;

    size = fread(&bmp_header, sizeof(bmp_header), 1, fd);
    if (size != sizeof(bmp_header)) { printf("Error while reading BMP header\n"); return; }

    size = fread(&dib_header, sizeof(dib_header), 1, fd);
    if (size != sizeof(dib_header)) { printf("Error while reading DIB header\n"); return; }

    int offset = bmp_header.offset_pixelarray;
    int width = dib_header.image_width;
    int height = dib_header.image_height;

    uint8_t rowbuf[512];

    int off_x = 0;
    int off_y = 0;

    fseek(fd, offset, SEEK_SET);

    // switch video mode and display image
    graphics_set_mode(GMODE_VIDEO);

    uint8_t *gbuff = (uint8_t*)(0xA0000);

    for (int y = 0; y < MIN(height, 240); y++) {
        size = fread(rowbuf, sizeof(uint8_t), width, fd);

        if (size != width) {
            graphics_set_mode(GMODE_TEXT);
            printf("Error while reading row %i.", y);
        }

        for (int x = 0; x < MIN(width, 320); x++) {
            int g_i = (239 - y) * 320 + x;

            gbuff[g_i] = rowbuf[x];
        }
    }

    while (1) {
        char c = getchar();
        if (c == 0x1B) { break; }
    }

    graphics_set_mode(GMODE_TEXT);
}