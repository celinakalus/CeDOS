#include "cedos.h"
#include "stdio.h"
#include "stdlib.h"
#include "string.h"

#include "assert.h"

#include <stdint.h>

#define GMODE_TEXT 0x02
#define GMODE_VIDEO 0x13

#define VGA_BUFFER (uint8_t*)(0xA0000)

#define VGA_MODE_13_WIDTH 320
#define VGA_MODE_13_HEIGHT 200

#define MAX(x, y) (x > y ? x : y)
#define MIN(x, y) (x < y ? x : y)

typedef struct __attribute__((packed)) {
    uint16_t signature;
    uint32_t file_size;
    uint16_t reserved_1;
    uint16_t reserved_2;
    uint32_t offset_pixelarray;
} BMP_FILE_HEADER;

/* Compression methods */
#define BI_RGB  0   // uncompressed
#define BI_RLE8 1   // RLE 8-bit/pixel
#define BI_RLE4 2   // RLE 4-bit/pixel
/* ... */

typedef struct {
    uint32_t DIB_header_size;
    uint32_t image_width;
    uint32_t image_height;
    uint16_t num_color_planes;
    uint16_t bits_per_pixel;
    uint32_t compression_method;
    uint32_t raw_data_size;
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

typedef struct {
    uint8_t* buffer;

    int width;
    int height;
    
    int x;
    int y;
} image_object;

static inline void imgbuf_newline(image_object* imgobj) {
    imgobj->x = 0;
    imgobj->y = MIN(imgobj->y + 1, imgobj->height - 1);
}

static inline void imgbuf_write_single(image_object* imgobj, uint8_t value) {
    if (imgobj->x >= imgobj->width) {
        imgbuf_newline(imgobj);
    }

    imgobj->buffer[imgobj->y * imgobj->width + imgobj->x] = value;
    imgobj->x++;
}

static inline void imgbuf_offset(image_object* imgobj, uint8_t xoff, uint8_t yoff) {
    imgobj->x += xoff;
    imgobj->y += yoff;
}

void decode_rle(uint8_t* rle_data, int rle_size, uint8_t* img_buf, int width, int height) {
    image_object imgobj = {
            .buffer = img_buf,
            .width = width,
            .height = height,
            .x = 0,
            .y = 0
        };

    int data_i = 0;

    while (1) {
        if (data_i >= rle_size) {
            break;
        }
        
        const int count = rle_data[data_i++];
        const int value = rle_data[data_i++];

        if (count == 0) {
            /* escape sequence */
            switch (value)
            {
            case 0:
                /* end of line */
                imgbuf_newline(&imgobj);
                break;
            
            case 1:
                /* end of bitmap; ignore for now */
                break;
            
            case 2:
                /* delta */
                uint8_t xoff = rle_data[data_i++];
                uint8_t yoff = rle_data[data_i++];
                imgbuf_offset(&imgobj, xoff, yoff);
                break;
            
            default:
                /* non-RLE subsequence */
                for (int i = 0; i < value; i++) {
                    imgbuf_write_single(&imgobj, rle_data[data_i++]);
                }
                data_i += value % 2;
                break;
            }
        } else {
            /* regular RLE content */
            for (int j = 0; j < count; j++) {
                imgbuf_write_single(&imgobj, value);
            }
        }
    }
}

void main(char *args) {
    // open image file
    const int fd = fopen(args, "r");

    if (fd < 0) {
        printf("Could not find file: %s\n", args);
        return;
    }

    const int limit = VGA_MODE_13_WIDTH * VGA_MODE_13_HEIGHT;

    const BMP_FILE_HEADER bmp_header;
    const DIB_HEADER dib_header;

    int size = 0;

    size = fread(&bmp_header, sizeof(bmp_header), 1, fd);
    if (size != sizeof(bmp_header)) { printf("Error while reading BMP header\n"); return; }

    size = fread(&dib_header, sizeof(dib_header), 1, fd);
    if (size != sizeof(dib_header)) { printf("Error while reading DIB header\n"); return; }

    const int offset = bmp_header.offset_pixelarray;
    const int width = dib_header.image_width;
    const int height = dib_header.image_height;

    assert(dib_header.bits_per_pixel == 8);
    assert(dib_header.num_color_planes == 1);

    const int data_size = width * height * (dib_header.bits_per_pixel / 8);
    char const* imgbuf = (char const*)malloc(data_size);
    memset(imgbuf, 0, data_size);
    
    fseek(fd, offset, SEEK_SET);

    if (dib_header.compression_method == BI_RLE8) {
        const int rle_size = dib_header.raw_data_size;
        char const* databuf = (char const*)malloc(rle_size);
        size = fread(databuf, sizeof(uint8_t), rle_size, fd);
        /* TODO: check returned size */

        decode_rle(databuf, size, imgbuf, width, height);

        free(databuf);
    } else if (dib_header.compression_method == BI_RGB) {
        size = fread(imgbuf, sizeof(uint8_t), data_size, fd);
        /* TODO: check returned size */
    }

    int off_x = 0;
    int off_y = 0;


    // switch video mode and display image
    graphics_set_mode(GMODE_VIDEO);

    uint8_t *gbuff = VGA_BUFFER;

    for (int y = 0; y < MIN(height, VGA_MODE_13_HEIGHT); y++) {
        for (int x = 0; x < MIN(width, VGA_MODE_13_WIDTH); x++) {
            int g_i = (VGA_MODE_13_HEIGHT - 1 - y) * VGA_MODE_13_WIDTH + x;

            gbuff[g_i] = imgbuf[y * width + x];
        }
    }

    free(imgbuf);

    while (1) {
        char c = getchar();
        if (c == 0x1B || c == 'q') { break; }
    }

    graphics_set_mode(GMODE_TEXT);
}