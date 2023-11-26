#include "cedos/mm/page_allocator.h"
#include "cedos/mm/paging.h"
#include "cedos/core.h"

uint8_t* first_free = (uint8_t*)0x00500000;

void* get_free_page() {
    void* res = first_free;
    first_free += PAGE_SIZE;
    return res;
}

uint32_t mem_usage(void) {
    return (uint32_t)(first_free);
}

void mark_as_free(void* page_addr) {
    
}