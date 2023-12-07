#ifndef FAT12_H
#define FAT12_H

#include <stdint.h>
#include <stddef.h>

typedef struct {
    // logical block addresses of FAT regions
    uint32_t FAT1_lba;
    uint32_t FAT2_lba;
    uint32_t root_lba;
    uint32_t data_lba;

    // FAT parameters, read out from boot sector
    uint32_t bytes_per_sect;
    uint32_t sect_per_cluster;
    uint32_t num_reserved_sectors;
    uint32_t num_FAT;
    uint32_t max_root_dir_entries;
    uint32_t total_log_sectors;
    uint32_t media_desc;
    uint32_t log_sect_per_fat;

    // calculated cluster size in bytes
    uint32_t cluster_size;
} FAT12_descriptor_t;


/**
 * @brief Read out a portion from the raw FAT12 image.
 * This function has to be defined by whichever code is including this driver.
 */
extern void *FAT12_read(uint32_t lba, uint32_t *offset, size_t size, void *buffer);


/**
 * @brief Initialize FAT12 descriptor structure
 * 
 * The FAT12 descriptor contains read-out and pre-computed
 * information about a given FAT12 image or disk.
 * FAT12_init initializes the descriptor \p fat based on the FAT
 * device that is accessible through the FAT12_read function.
 * 
 * @param fat FAT descriptor to be initialized.
*/
void FAT12_init(FAT12_descriptor_t *fat);

/**
 * @brief Read a single cluster from the FAT image.
 * 
 * @param fat FAT descriptor of the FAT device. The driver user has to
 * ensure that the descriptor corresponds to the FAT device accessible
 * through the FAT12_read function.
 * @param cluster Index of the cluster to read.
 * @param buffer Pointer to a cluster-sized buffer.
*/
void *FAT12_read_cluster(FAT12_descriptor_t *fat, uint16_t cluster, void *buffer);

/**
 * @brief Iterate through entries in the root directory and return the next file name, the first cluster index, and file size.
 * 
 * @param fat FAT descriptor of the FAT device.
 * @param index Index within the root directory table at which to start.
 * @param fname_buffer Pointer to a buffer for the file name.
 * @param first_cluster Pointer to a uint16_t to be filled with the index of the first cluster of the file.
 * @param file_size Pointer to a uint32_t to be filled with the file size.
 * 
 * @return -1 if the search has reached an end; Index within the directory table otherwise.
*/
int FAT12_root_dir_next(FAT12_descriptor_t *fat, int index, char *fname_buffer, uint16_t *first_cluster, uint32_t *file_size);

/**
 * @brief Given a cluster index, return the value of the FAT table.
 * 
 * @param fat FAT descriptor of the FAT device.
 * @param cluster Cluster index.
 *
 * @return Value of the FAT at the given cluster index.
*/
uint16_t FAT12_next_cluster(FAT12_descriptor_t *fat, uint16_t cluster);

#endif