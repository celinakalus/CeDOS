#ifndef FAT_H
#define FAT_H

#include <stdint.h>

typedef struct {
    char jmp[3];
    char oemname[8];

    uint16_t bytes_per_sect;
    uint8_t sect_per_cluster;
    uint16_t num_reserved_sectors;
    uint8_t num_FAT;
    uint16_t max_root_dir_entries;
} BOOT_SECT;

#endif