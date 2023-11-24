#include "cedos/file.h"
#include "cedos/fat.h"
#include "string.h"
#include "assert.h"

#include "cedos/mm/memory.h"

#include <stdint.h>

file_operations_t FAT_fops = {
    NULL,           /* open */
    FAT_openat,     /* openat */
    FAT_read,       /* read */
    NULL,           /* write */
    FAT_dir_next,   /* dir_next */
    FAT_lseek,      /* lseek */
    FAT_tell        /* tell */
};

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


void *FAT_addr;
BOOT_SECT *boot_sect;

uint32_t FAT1_lba;
uint32_t FAT2_lba;
uint32_t root_lba;
uint32_t data_lba;

void FAT_init() {
    const int sector_size = 512;
    const int sector_num = 128;

    // open image file
    FAT_addr = (void*)(0x10000);

    boot_sect = (BOOT_SECT*)(FAT_addr);

    FAT1_lba = boot_sect->num_reserved_sectors;
    FAT2_lba = FAT1_lba + boot_sect->log_sect_per_fat;
    root_lba = FAT1_lba + (boot_sect->log_sect_per_fat * boot_sect->num_FAT);

    long root_dir_size = boot_sect->max_root_dir_entries * sizeof(DIR_ENTRY);
    data_lba = root_lba + (root_dir_size / boot_sect->bytes_per_sect);
}

void *FAT_read_sector_offset(uint32_t lba, uint32_t *offset) {
    if (offset != NULL) {
        lba += (*offset) / boot_sect->bytes_per_sect;
        *offset = (*offset) % boot_sect->bytes_per_sect;
    }

    return (void*)((long)(FAT_addr) + (long)(lba * boot_sect->bytes_per_sect));
}

void *FAT_read_cluster(uint16_t cluster, void *buffer) {
    // TODO: perform memcpy
    void *addr = FAT_read_sector_offset(data_lba + ((cluster - 2) * boot_sect->sect_per_cluster), NULL);

    uint32_t cluster_size = boot_sect->bytes_per_sect * boot_sect->sect_per_cluster;

    memcpy(buffer, addr, cluster_size);

    return (void*)((uint8_t*)(buffer) + cluster_size);
}

int FAT_root_dir_next(int index, char *fname_buffer, uint16_t *first_cluster, uint32_t *file_size) {
    //memset(fname_buffer, 0, sizeof(fname_buffer));

    while (1) {
        // index overflow
        if (index >= boot_sect->max_root_dir_entries) {
            return -1;
        }

        uint32_t offset = index * sizeof(DIR_ENTRY);
        void *sect = FAT_read_sector_offset(root_lba, &offset);
        DIR_ENTRY *dir_entry = (DIR_ENTRY *)((uint32_t)(sect) + offset);

        // if first character of name is 0, then no subsequent entry is in use
        if (dir_entry->name[0] == 0x00) { 
            return -1; 
        }

        // deleted file
        if (dir_entry->name[0] == (char)(0xE5)) {
            index++;
            continue;
        }

        // VFAT LFN entry
        if (dir_entry->file_attr == 0x0F && dir_entry->start_of_clusters == 0 && dir_entry->file_size != 0) {
            VFAT_LFN_ENTRY *lfn_entry = (VFAT_LFN_ENTRY*)(dir_entry);

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

            index++;
            continue;
        }

        if (index == 0 && (dir_entry->file_attr & 0x08) && dir_entry->file_size == 0) {
            // volume label
            index++;
            continue;

        } else if ((dir_entry->file_attr & 0x10) && dir_entry->file_size == 0) {
        // subdirectory

        } else {
        // regular file

        }

        *file_size = dir_entry->file_size;
        *first_cluster = dir_entry->start_of_clusters;

        // if no VFAT LFN exists, use DOS name
        if (fname_buffer[0] == 0) {
            for (int i = 0; i < 8; i++) {
                fname_buffer[i] = dir_entry->name[i];
            }
            fname_buffer[8] = '.';
            for (int i = 0; i < 3; i++) {
                fname_buffer[i + 9] = dir_entry->ext[i];
            }
            fname_buffer[12] = 0;
        }

        return index + 1;
    }
}

int FAT_dir_next(file_t *file, int index, char *fname_buffer) {
    uint16_t first_cluster;
    uint32_t file_size;

    // TODO: subdirectories
    return FAT_root_dir_next(index, fname_buffer, &first_cluster, &file_size);
}

uint16_t FAT_next_cluster(uint16_t cluster) {
    // assuming FAT12
    uint32_t *offset = (cluster >> 1) * 3;
    uint8_t *sect = FAT_read_sector_offset(FAT1_lba, &offset);
    sect += (uint32_t)(offset);

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

int FAT_openat(file_t *root, file_t *handle, const char *fname, int flags) {
    int i = 0;

    // TODO: take fd into consideration (open file in that subdirectory)
    uint16_t first_cluster;
    while (1) {
        char buffer[832];
        uint32_t file_size;

        i = FAT_root_dir_next(i, buffer, &first_cluster, &file_size);
        if (i <= 0) { return -1; }

        if (strcmp(buffer, fname) == 0) {
            // file found
            handle->pos = 0;
            handle->size = file_size;
            handle->fops = &FAT_fops;
            handle->fat_cluster = first_cluster;
            return 0;
        }
    }
}

uint32_t FAT_read(file_t *file, uint8_t *buffer, uint32_t count) {
    uint16_t cluster = file->fat_cluster;
    fpos_t offset = file->pos;
    size_t file_size = file->size;
    uint32_t size = 0;

    uint32_t cluster_size = boot_sect->bytes_per_sect * boot_sect->sect_per_cluster;
    uint8_t *cluster_buffer = os_kernel_malloc(cluster_size);

    if (offset + count > file_size) {
        count = file_size - offset;
    }

    while (offset >= cluster_size) {
        cluster = FAT_next_cluster(cluster);
        if (cluster == 0xFFF || cluster == 0x000) { return -1; }
        offset -= cluster_size;
    }

    while (count > 0) {
        if (cluster == 0xFFF || cluster == 0x000) { break; }

        FAT_read_cluster(cluster, cluster_buffer);
        cluster = FAT_next_cluster(cluster);

        uint32_t memcpy_size;

        if (offset + count > cluster_size) {
            memcpy_size = (cluster_size - offset);
        } else {
            memcpy_size = count;
        }

        memcpy(buffer, (cluster_buffer + offset), memcpy_size);

        offset = 0;
        count -= memcpy_size;
        buffer += memcpy_size;
        size += memcpy_size;
    }

    file->pos += size;

    return size;
}

off_t FAT_lseek(file_t *file, off_t offset, int whence) {
    if (whence == SEEK_SET) {
        file->pos = offset;
    } else if (whence == SEEK_CUR) {
        file->pos += offset;
    } else if (whence == SEEK_END) {
        // to be implemented
    } else {
        kpanic("Wrong whence!");
    }
}

off_t FAT_tell(file_t *file) {
    return file->pos;
}