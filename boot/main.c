#include <stdint.h>
#include <stddef.h>

#include "string.h"
#include "fat12.h"

void printc(char c) {
    static uint8_t *display = (uint8_t*)(0xB8000);
    *(display++) = c;
    *(display++) = 0x0F;
}

void print_string(char *str) {
    while (*str != 0) {
        printc(*(str++));
    }
}

void assert_failed(const char * message) {
    print_string(message);

    while (1) {}
}

FAT12_descriptor_t fat_desc;

void *FAT12_read(uint32_t lba, uint32_t *offset, size_t size, void *buffer) {
    void *FAT_addr = (void*)(0x10000);

    if (offset != NULL) {
        lba += (*offset) / fat_desc.bytes_per_sect;
        *offset = (*offset) % fat_desc.bytes_per_sect;

        void *ptr = (void*)((long)(FAT_addr) + (long)(lba * fat_desc.bytes_per_sect));
        return memcpy(buffer, ptr + *offset, size);
    } else {
        void *ptr = (void*)((long)(FAT_addr) + (long)(lba * fat_desc.bytes_per_sect));
        return memcpy(buffer, ptr, size);
    }
}

void *FAT12_write(uint32_t lba, uint32_t *offset, size_t size, void *buffer) {
    void *FAT_addr = (void*)(0x10000);

    if (offset != NULL) {
        lba += (*offset) / fat_desc.bytes_per_sect;
        *offset = (*offset) % fat_desc.bytes_per_sect;

        void *ptr = (void*)((long)(FAT_addr) + (long)(lba * fat_desc.bytes_per_sect));
        return memcpy(ptr + *offset, buffer, size);
    } else {
        void *ptr = (void*)((long)(FAT_addr) + (long)(lba * fat_desc.bytes_per_sect));
        return memcpy(ptr, buffer, size);
    }
}

int load_kernel() {
    // debug output
    uint8_t *dbuf = (uint8_t *)(0x10000);
    for (int i = 0; i < 16; i++) {
        uint8_t value = dbuf[i];
        char lut[] = "0123456789ABCDEF";

        uint8_t low = value & 0x0F;
        uint8_t high = value >> 4;

        printc(lut[high]);
        printc(lut[low]);
        printc(' ');
    }

    //while (1);
    FAT12_init(&fat_desc);

    int i = 0;

    // first cluster of kernel.bin file
    uint16_t first_cluster;

    while (1) {
        char buffer[832];
        uint32_t file_size;

        i = FAT12_root_dir_next(&fat_desc, i, buffer, &first_cluster, &file_size);

        print_string(buffer);
        print_string("  ");
        if (i <= 0) { return -1; }

        if (!(strcmp(buffer, "kernel.bin"))) { break; }
    }

    // copy all clusters
    uint16_t cluster = first_cluster;
    uint8_t *buffer = (uint8_t *)(0x100000);
    while (1) {
        buffer = FAT12_read_cluster(&fat_desc, cluster, buffer);
        cluster = FAT12_next_cluster(&fat_desc, cluster);
        
        if (cluster == 0xFFF || cluster == 0x000) { break; }
    }

    return 0;
}