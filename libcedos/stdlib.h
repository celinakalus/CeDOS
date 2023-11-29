/*! \file
 * Provides functions for memory allocation.
 */
#ifndef MEMORY_H
#define MEMORY_H

#include <stdint.h>

typedef uint32_t size_t;

#define NULL (void*)(0)

/*!
 * Initialize memory allocator with predefined memory region.
 * \param start Start of malloc memory region.
 * \param end End of malloc memory region.
 * \return Currently always 0.
 */
int malloc_init(void *start, void *end);

/*!
 * Allocates a block of \p size bytes of memory.
 * \param size Size in bytes of the requested block of memory.
 * \return Memory address to the new memory block
 */
void* malloc(size_t size);

/*!
 * Frees a previously allocated block of memory.
 * \param ptr Pointer to the memory block to be freed.
 */
void free(void* ptr);


#endif
