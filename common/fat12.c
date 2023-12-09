#include "fat12.h"

#include <stdint.h>
#include <stddef.h>

#include "string.h"

typedef struct {
    char jmp[3];
    char oemname[8];

    uint16_t bytes_per_sect;
    uint8_t sect_per_cluster;
    uint16_t num_reserved_sectors;
    uint8_t num_FAT;
    uint16_t max_root_dir_entries;
    uint16_t total_log_sectors;
    uint8_t media_desc;
    uint16_t log_sect_per_fat;
} __attribute__((packed))  BOOT_SECT;

typedef struct {
    char name[8];
    char ext[3];
    uint8_t file_attr;
    uint8_t user_attr;
    uint8_t del_char;
    uint16_t create_time;
    uint16_t create_date;
    uint16_t last_access_date;
    uint16_t access_rights;
    uint16_t last_modified_time;
    uint16_t last_modified_date;
    uint16_t start_of_clusters;
    uint32_t file_size;
} __attribute__((packed)) DIR_ENTRY;

typedef struct {
    uint8_t seq_num;
    uint16_t part_1[5];
    uint8_t file_attr;
    uint8_t user_attr;
    uint8_t del_char;
    uint16_t part_2[6];
    uint16_t start_of_clusters;
    uint16_t part_3[2];
} __attribute__((packed)) VFAT_LFN_ENTRY;


/**
 * @brief 
*/
void FAT12_init(FAT12_descriptor_t *fat) {
    BOOT_SECT boot_sect;
    FAT12_read(0, NULL, sizeof(boot_sect), &boot_sect);

    // copy FAT parameters from boot sector into fat descriptor
    fat->bytes_per_sect         = boot_sect.bytes_per_sect;
    fat->log_sect_per_fat       = boot_sect.log_sect_per_fat;
    fat->max_root_dir_entries   = boot_sect.max_root_dir_entries;
    fat->media_desc             = boot_sect.media_desc;
    fat->num_FAT                = boot_sect.num_FAT;
    fat->num_reserved_sectors   = boot_sect.num_reserved_sectors;
    fat->sect_per_cluster       = boot_sect.sect_per_cluster;
    fat->total_log_sectors      = boot_sect.total_log_sectors;

    // calculate starting addresses of FAT regions
    fat->FAT1_lba = fat->num_reserved_sectors;
    fat->FAT2_lba = fat->FAT1_lba + fat->log_sect_per_fat;
    fat->root_lba = fat->FAT1_lba + (fat->log_sect_per_fat * fat->num_FAT);

    long root_dir_size = fat->max_root_dir_entries * sizeof(DIR_ENTRY);
    fat->data_lba = fat->root_lba + (root_dir_size / fat->bytes_per_sect);
    
    // calculate cluster size
    fat->cluster_size = fat->bytes_per_sect * fat->sect_per_cluster;
}

void *FAT12_read_cluster(FAT12_descriptor_t *fat, uint16_t cluster, void *buffer) {
    FAT12_read(fat->data_lba + ((cluster - 2) * fat->sect_per_cluster), NULL, fat->cluster_size, buffer);

    return buffer += fat->cluster_size;
}

int FAT12_root_dir_next(FAT12_descriptor_t *fat, int index, char *fname_buffer, uint16_t *first_cluster, uint32_t *file_size) {
    fname_buffer[0] = 0;
    
    while (1) {
        //printk("%i\n", index);
        // index overflow
        if (index >= fat->max_root_dir_entries) {
            return -1;
        }

        uint32_t offset = index * sizeof(DIR_ENTRY);
        DIR_ENTRY dir_entry;
        FAT12_read(fat->root_lba, &offset, sizeof(DIR_ENTRY), &dir_entry);

        // if first character of name is 0, then no subsequent entry is in use
        if (dir_entry.name[0] == 0x00) { 
            return -1; 
        }

        // deleted file
        if (dir_entry.name[0] == (char)(0xE5)) {
            index++;
            continue;
        }

        // VFAT LFN entry
        if (dir_entry.file_attr == 0x0F && dir_entry.start_of_clusters == 0 && dir_entry.file_size != 0) {
            VFAT_LFN_ENTRY *lfn_entry = (VFAT_LFN_ENTRY*)(&dir_entry);

            int offset = 13 * ((lfn_entry->seq_num & 0x3F) - 1);

            // read long file name
            for (int i = 0; i < 5; i++) {
                fname_buffer[offset++] = lfn_entry->part_1[i];
            }

            for (int i = 0; i < 6; i++) {
                fname_buffer[offset++] = lfn_entry->part_2[i];
            }

            for (int i = 0; i < 2; i++) {
                fname_buffer[offset++] = lfn_entry->part_3[i];
            }

            if (lfn_entry->seq_num & (1 << 6)) {
                fname_buffer[offset] = 0;
            }

            index++;
            continue;
        }

        if (index == 0 && (dir_entry.file_attr & 0x08) && dir_entry.file_size == 0) {
            // volume label
            index++;
            continue;

        } else if ((dir_entry.file_attr & 0x10) && dir_entry.file_size == 0) {
        // subdirectory

        } else {
        // regular file

        }

        *file_size = dir_entry.file_size;
        *first_cluster = dir_entry.start_of_clusters;

        // if no VFAT LFN exists, use DOS name
        if (fname_buffer[0] == 0) {
            memcpy(fname_buffer, dir_entry.name, 8);
            fname_buffer[8] = '.';
            memcpy(fname_buffer + 9, dir_entry.ext, 3);
            fname_buffer[12] = 0;
        }

        return index + 1;
    }
}

uint16_t FAT12_next_cluster(FAT12_descriptor_t *fat, uint16_t cluster) {
    // assuming FAT12
    uint32_t offset = (cluster >> 1) * 3;
    uint8_t sect[3];
    FAT12_read(fat->FAT1_lba, &offset, sizeof(sect), sect);

    if (cluster & 0x01) {
        uint16_t high = (uint16_t)(sect[2]);
        uint16_t low = (uint16_t)(sect[1] & 0xF0) >> 4;
        return (high << 4) | low;
    } else {
        uint16_t low = (uint16_t)(sect[0]);
        uint16_t high = (uint16_t)(sect[1] & 0x0F) << 8;
        return low | high;
    }
}