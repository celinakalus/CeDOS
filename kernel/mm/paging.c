#include "mm/paging.h"
#include "mm/page_allocator.h"
#include "linker.h"
#include "string.h"
#include "interrupts.h"
#include "core.h"

#define MAKE_PAGE_ENTRY(addr, flags) (uint32_t)(((uint32_t)(addr) & 0xFFFFF000) | (flags))

#define PAGE_DIRECTORY (PAGE_DIR_ENTRY[PAGE_ENTRY_COUNT])
#define PAGE_TABLE (PAGE_TABLE_ENTRY[PAGE_ENTRY_COUNT])

#define PAGE_DIR_INDEX(addr) (((uint32_t)(addr) >> 22) % PAGE_ENTRY_COUNT)
#define PAGE_TABLE_INDEX(addr) (((uint32_t)(addr) >> 12) % PAGE_ENTRY_COUNT)

#define PAGE_MAPPED_ADDR(dir_index, table_index) (void*)((dir_index * PAGE_ENTRY_COUNT + table_index) * PAGE_SIZE)

#define PAGE_TABLE_MAPPED_ADDR(index) \
        ((PAGE_TABLE_ENTRY*)(PAGE_MAPPED_ADDR((PAGE_ENTRY_COUNT - 1), index)))
#define PAGE_DIR_MAPPED_ADDR \
        ((PAGE_DIR_ENTRY*)(PAGE_TABLE_MAPPED_ADDR(PAGE_ENTRY_COUNT - 1)))

#define PAGE_TABLE_ALT_MAPPED_ADDR(index) \
        ((PAGE_TABLE_ENTRY*)(PAGE_MAPPED_ADDR((PAGE_ENTRY_COUNT - 2), index)))
#define PAGE_DIR_ALT_MAPPED_ADDR \
        ((PAGE_DIR_ENTRY*)(PAGE_TABLE_ALT_MAPPED_ADDR(PAGE_ENTRY_COUNT - 1)))
        
#define PAGE_TABLE_FLAGS 0b000000000011

#ifdef DEBUG
#define PRINT_DBG(...) printk("[" __FILE__ "] " __VA_ARGS__)
#else
#define PRINT_DBG(...) {}
#endif
        
__attribute__((always_inline)) inline int is_present(uint32_t entry) {
    return (entry & 0b000000000001);
}
        
/*!
* WARNING: This method assumes the target page directory has already been mounted
* to the second to last page directory entry
*/
__attribute__((always_inline)) inline int is_addr_available(uint32_t dir_index, uint32_t table_index) {
    PAGE_DIR_ENTRY* page_dir = PAGE_DIR_ALT_MAPPED_ADDR;
    PAGE_TABLE_ENTRY* page_table = PAGE_TABLE_ALT_MAPPED_ADDR(table_index);
    
    if(!is_present(page_dir[dir_index])) {
        return 1;
    } else if (!is_present(page_table[table_index])) {
        return 1;
    } else {
        return 0;
    }
}

void mount_page_dir(PHYS_ADDR page_dir) {
    PAGE_DIR_ENTRY *cur_page_dir = PAGE_DIR_MAPPED_ADDR;
    cur_page_dir[PAGE_ENTRY_COUNT - 2] = MAKE_PAGE_ENTRY(page_dir, PAGE_TABLE_FLAGS);
}

/*!
* WARNING: This method assumes the target page directory has already been mounted
* to the second to last page directory entry
*/
int force_map_page_to(PHYS_ADDR page_addr, uint32_t dir_index, uint32_t table_index, uint32_t flags) {
    PAGE_DIR_ENTRY* page_dir = PAGE_DIR_ALT_MAPPED_ADDR;
    PAGE_TABLE_ENTRY* page_table = PAGE_TABLE_ALT_MAPPED_ADDR(dir_index);

    if (!is_present(page_dir[dir_index])) {
        // acquire new page table
        PHYS_ADDR new_page_table = get_free_page();
        page_dir[dir_index] = MAKE_PAGE_ENTRY(new_page_table, PAGE_TABLE_FLAGS);
        inv_all_pages();
        memset(page_table, 0, PAGE_SIZE);
    }

    // map page
    page_table[table_index] = MAKE_PAGE_ENTRY(page_addr, flags);
    inv_all_pages();
    return 1;
}

int map_page_to(PHYS_ADDR page_addr, uint32_t dir_index, uint32_t table_index, uint32_t flags) {
    if (is_addr_available(dir_index, table_index)) {
        return force_map_page_to(page_addr, dir_index, table_index, flags);
    } else {
        return 0;
    }
}

int force_map_page_to_this(PHYS_ADDR page_addr, uint32_t dir_index, uint32_t table_index, uint32_t flags) {
    PAGE_DIR_ENTRY* page_dir = PAGE_DIR_MAPPED_ADDR;
    PAGE_TABLE_ENTRY* page_table = PAGE_TABLE_MAPPED_ADDR(dir_index);

    if (!is_present(page_dir[dir_index])) {
        // acquire new page table
        void *new_page_table = get_free_page();
        page_dir[dir_index] = MAKE_PAGE_ENTRY(new_page_table, PAGE_TABLE_FLAGS);
        inv_all_pages();
        memset(page_table, 0, PAGE_SIZE);
    }

    // map page
    page_table[table_index] = MAKE_PAGE_ENTRY(page_addr, flags);
    inv_all_pages();
    return 1;
}

int map_page_to_this(PHYS_ADDR page_addr, uint32_t dir_index, uint32_t table_index, uint32_t flags) {
    if (is_addr_available(dir_index, table_index)) {
        return force_map_page_to_this(page_addr, dir_index, table_index, flags);
    } else {
        return 0;
    }
}

size_t copy_to_pdir(VIRT_ADDR src, size_t length, PHYS_ADDR pdir, VIRT_ADDR dest) {
    VIRT_ADDR mount_dest = (VIRT_ADDR)(0xe0000000);
    mount_page_dir(pdir);
    PHYS_ADDR page;

    while (length != 0) {
        page = get_free_page();
        force_map_page_to_this(page, PAGE_DIR_INDEX(mount_dest), PAGE_TABLE_INDEX(mount_dest), PAGE_TABLE_FLAGS);
        force_map_page_to(page, PAGE_DIR_INDEX(dest), PAGE_TABLE_INDEX(dest), PAGE_TABLE_FLAGS);

        uint32_t offset = ((uint32_t)dest % PAGE_SIZE);
        uint32_t part_length = (offset + length <= PAGE_SIZE) ? length : PAGE_SIZE - offset;

        PRINT_DBG("src=%p dest=%p length=%i offset=%i plen=%i\n", src, dest, length, offset, part_length);

        memset(mount_dest, 0, PAGE_SIZE);
        memcpy((uint8_t*)(mount_dest) + offset, src, part_length);

        dest += part_length;
        src += part_length;
        length -= part_length;
    }

    return dest;
}

int map_range_to(PHYS_ADDR page_dir, VIRT_ADDR dest, PHYS_ADDR src, uint32_t page_count, uint32_t flags) {
    mount_page_dir(page_dir);

    
    for (uint32_t i = 0; i < page_count; i++) {
        uint32_t dir_index = PAGE_DIR_INDEX(dest + i * PAGE_SIZE);
        uint32_t table_index = PAGE_TABLE_INDEX(dest + i * PAGE_SIZE);
        
        if (!is_addr_available(dir_index, table_index)) { return 0; }
    }
    
    for (uint32_t i = 0; i < page_count; i++) {
        uint32_t dir_index = PAGE_DIR_INDEX(dest + i * PAGE_SIZE);
        uint32_t table_index = PAGE_TABLE_INDEX(dest + i * PAGE_SIZE);
        
        map_page_to(src + i * PAGE_SIZE, dir_index, table_index, flags);
    }

    return 1;
}

VIRT_ADDR map_first_free(PHYS_ADDR page_dir, VIRT_ADDR search_start, PHYS_ADDR page, uint32_t flags) {
    mount_page_dir(page_dir);
    uint32_t table_start_i = PAGE_TABLE_INDEX(search_start);

    for (uint32_t dir_i = PAGE_DIR_INDEX(search_start); dir_i < (PAGE_ENTRY_COUNT - 2); dir_i++) {
        for (uint32_t table_i = table_start_i; table_i < PAGE_ENTRY_COUNT; table_i++) {
            if (map_page_to(page, dir_i, table_i, flags)) {
                return PAGE_MAPPED_ADDR(dir_i, table_i);
            }
        }

        table_start_i = 0;
    }

    return 0;
}

PHYS_ADDR create_empty_page_dir(void) {
    PHYS_ADDR page_dir_phys = get_free_page();
    mount_page_dir(page_dir_phys);
    PAGE_DIR_ENTRY *page_dir = (VIRT_ADDR)0xffffe000; //PAGE_MAPPED_ADDR(PAGE_ENTRY_COUNT - 1, PAGE_ENTRY_CPAGE_MAPPED_ADDR(PAGE_ENTRY_COUNT - 1, PAGE_ENTRY_COUNT - 2)OUNT - 2);
    inv_all_pages();

    memset(page_dir, 0, PAGE_SIZE);
    
    // map to itself
    page_dir[PAGE_ENTRY_COUNT - 1] = MAKE_PAGE_ENTRY(page_dir_phys, PAGE_TABLE_FLAGS);
    
    page_dir = PAGE_DIR_ALT_MAPPED_ADDR;

    // identity map first 4MB
    page_dir[0] = PAGE_DIR_MAPPED_ADDR[0];

    // map kernel
    page_dir[PAGE_DIR_INDEX(0xc0000000)] = PAGE_DIR_MAPPED_ADDR[PAGE_DIR_INDEX(0xc0000000)];
    page_dir[PAGE_DIR_INDEX(0xc0400000)] = PAGE_DIR_MAPPED_ADDR[PAGE_DIR_INDEX(0xc0400000)];

    return page_dir_phys;
}

#define PAGE_FAULT_FLAGS_PRESENT (1 << 0)

EXCEPTION(page_fault_isr, frame, error_code) {
    volatile VIRT_ADDR faulty_addr;
    __asm__ volatile ("mov %%cr2, %0" : "=a" (faulty_addr));
    uint32_t pdir_index = PAGE_DIR_INDEX(faulty_addr);
    uint32_t ptbl_index = PAGE_TABLE_INDEX(faulty_addr);

    if (error_code & PAGE_FAULT_FLAGS_PRESENT) {
        kpanic("Page-protection violation!");
    } else {
        PHYS_ADDR new_page = get_free_page();
        force_map_page_to_this(new_page, pdir_index, ptbl_index, PAGE_TABLE_FLAGS);
        memset(PAGE_MAPPED_ADDR(pdir_index, ptbl_index), 0, PAGE_SIZE);
    }
}

int paging_init(void) {
    install_interrupt(0x0e, page_fault_isr, 0x18, TRAP_GATE);
    return 0;
}