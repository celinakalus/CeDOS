#include "interrupts.h"
#include "core.h"
#include "sched/sched.h"
#include "file.h"
#include "drivers/graphics.h"
#include "time.h"
#include "mm/page_allocator.h"

void test(uint32_t ebx, uint32_t ecx, uint32_t edx) {
    printk("SYSCALL 0x01: EBX=%i ECX=%X EDX=%X\n", ebx, ecx, edx);
}

void* SYSCALL_TABLE[] = { 
        file_read,
        file_write,
        sched_yield, 
        get_current_process,
        sched_spawn,
        sched_wait,
        file_open,
        graphics_set_mode,
        hard_reset,
        file_dir_next,
        file_lseek,
        file_tell,
        time_get_ticks,
        mem_usage,
        sched_kill
    };

extern void syscall_interrupt(void);

/*!
 * Installs the syscall interrupt to INT 0x30
 */
int syscall_init(void) {
    install_interrupt(0x30, &syscall_interrupt, 0x18, INT_GATE);
    return 1;
}