#include "file.h"
#include "fat.h"
#include "string.h"
#include "assert.h"
#include "core.h"

#include "memory.h"
#include "fat12.h"

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

uint8_t *cluster_buffer;

FAT12_descriptor_t *fat_desc;

void *FAT12_read(uint32_t lba, uint32_t *offset, size_t size, void *buffer) {
    void *FAT_addr = (void*)(0x10000);

    if (offset != NULL) {
        lba += (*offset) / fat_desc->bytes_per_sect;
        *offset = (*offset) % fat_desc->bytes_per_sect;

        void *ptr = (void*)((long)(FAT_addr) + (long)(lba * fat_desc->bytes_per_sect));
        return memcpy(buffer, ptr + *offset, size);
    } else {
        void *ptr = (void*)((long)(FAT_addr) + (long)(lba * fat_desc->bytes_per_sect));
        return memcpy(buffer, ptr, size);
    }
}


void FAT_init() {
    fat_desc = malloc(sizeof(FAT12_descriptor_t));

    FAT12_init(fat_desc);

    cluster_buffer = malloc(fat_desc->cluster_size);
}


int FAT_dir_next(file_t *file, int index, char *fname_buffer) {
    uint16_t first_cluster;
    uint32_t file_size;

    // TODO: subdirectories
    return FAT12_root_dir_next(fat_desc, index, fname_buffer, &first_cluster, &file_size);
}

int FAT_openat(file_t *root, file_t *handle, const char *fname, int flags) {
    int i = 0;

    // TODO: take fd into consideration (open file in that subdirectory)
    uint16_t first_cluster;
    while (1) {
        char buffer[832];
        uint32_t file_size;

        i = FAT12_root_dir_next(fat_desc, i, buffer, &first_cluster, &file_size);
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

    if (offset + count > file_size) {
        count = file_size - offset;
    }

    while (offset >= fat_desc->cluster_size) {
        cluster = FAT12_next_cluster(fat_desc, cluster);
        if (cluster == 0xFFF || cluster == 0x000) { return -1; }
        offset -= fat_desc->cluster_size;
    }

    while (count > 0) {
        if (cluster == 0xFFF || cluster == 0x000) { break; }

        FAT12_read_cluster(fat_desc, cluster, cluster_buffer);
        cluster = FAT12_next_cluster(fat_desc, cluster);

        uint32_t memcpy_size;

        if (offset + count > fat_desc->cluster_size) {
            memcpy_size = (fat_desc->cluster_size - offset);
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