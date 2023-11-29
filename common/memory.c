#include "memory.h"
#include "string.h"

#include "assert.h"

struct memblock {
    struct memblock *next;
    size_t size;
};

volatile struct memblock volatile *malloc_first, *malloc_last, *malloc_next_free;

int malloc_init(void *start, void *end) {
    uint32_t mem_start = (uint32_t)(start);
    uint32_t mem_end = (uint32_t)(end);

    malloc_first = (volatile struct memblock*)(mem_start);
    malloc_last = (volatile struct memblock*)(mem_end - sizeof(struct memblock));
    malloc_next_free = malloc_first;

    malloc_first->size = 0;
    malloc_first->next = malloc_last;

    malloc_last->size = 0;
    malloc_last->next = NULL;

    return 0;
}

static struct memblock* get_memblock_from_ptr(void* ptr) {
    /* Just search through all blocks */
    struct memblock* next = malloc_first;
    uint32_t target = (uint32_t)(ptr);

    while (next) {
        uint32_t start = (uint32_t)(&next[1]);
        uint32_t end = (uint32_t)(next->next);

        if (target >= start && target < end) {
            return next;
        }

        next = next->next;
    }

    return NULL;
}

/*!
 * Allocates a block of \p size bytes of memory. (KERNEL MODE)
 * \param size Size in bytes of the requested block of memory.
 * \return Memory address to the new memory block
 */
void* malloc(size_t size) {
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

void* realloc(void* ptr, size_t new_size) {
    void* new_ptr = malloc(new_size);
    struct memblock* block = get_memblock_from_ptr(ptr);

    if (block == NULL) {
        /* corresponding block not found */
        return NULL;
    }
    if (block->size == 0) {
        /* corresponding block was already freed */
        return NULL;
    }

    memcpy(new_ptr, ptr, block->size);
    free(ptr);

    return new_ptr;
}

/*!
 * Frees a previously allocated block of memory. (KERNEL MODE)
 * \param ptr Pointer to the memory block to be freed.
 */
void free(void* ptr) {
    
}
