#ifndef FAT_H
#define FAT_H

#include <stdint.h>

void FAT_init();
void *FAT_read_sector_offset(uint32_t lba, uint32_t *offset);
void *FAT_read_cluster(uint16_t cluster, void *buffer);
uint16_t FAT_next_cluster(uint16_t cluster);
int FAT_root_dir_next(int index, char *fname_buffer, uint16_t *first_cluster, uint32_t *file_size);
void *FAT_find_file(const char *fname);

#endif