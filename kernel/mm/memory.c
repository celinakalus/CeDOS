#include "mm/memory.h"

#include "assert.h"

struct memblock {
    struct memblock *next;
    int size;
};

volatile struct memblock volatile *malloc_first, *malloc_last, *malloc_next_free;

int malloc_init() {
    uint32_t mem_start = 0xC0400000u;
    uint32_t mem_end = 0xC0800000u;

    malloc_first = (volatile struct memblock*)(mem_start);
    malloc_last = (volatile struct memblock*)(mem_end - sizeof(struct memblock));
    malloc_next_free = malloc_first;

    malloc_first->size = 0;
    malloc_first->next = malloc_last;

    malloc_last->size = 0;
    malloc_last->next = NULL;
}

/*!
 * Allocates a block of \p size bytes of memory. (KERNEL MODE)
 * \param size Size in bytes of the requested block of memory.
 * \return Memory address to the new memory block
 */
void* os_kernel_malloc(size_t size) {
    // size == 0 means the block is free
    assert(malloc_next_free->size == 0);

    // TODO: test if memory block is large enough


    uint32_t addr = (uint32_t)(malloc_next_free);
    addr += sizeof(struct memblock);
    addr += size;

    // TODO: in some cases, one might rather link to the
    //  next block after that instead of creating a new block

    struct memblock *new_block = (volatile struct memblock*)(addr);
    
    new_block->next = malloc_next_free->next;
    new_block->size = 0;

    malloc_next_free->next = new_block;
    malloc_next_free->size = size;

    void *pointer = (void*)(&malloc_next_free[1]);
    malloc_next_free = new_block;

    return pointer;
}

/*!
 * Frees a previously allocated block of memory. (KERNEL MODE)
 * \param ptr Pointer to the memory block to be freed.
 */
void os_kernel_free(void* ptr) {
    
}

/*!
 * Allocates a block of \p size bytes of memory. (USER MODE)
 * \param size Size in bytes of the requested block of memory.
 * \return Memory address to the new memory block
 */
void* os_user_malloc(size_t size) {
    return (void*)0;
}

/*!
 * Frees a previously allocated block of memory. (USER MODE)
 * \param ptr Pointer to the memory block to be freed.
 */
void os_user_free(void* ptr) {
    
}