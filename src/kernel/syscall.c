#include "cedos/interrupts.h"
#include "cedos/core.h"
#include "cedos/sched/sched.h"
#include "cedos/file.h"
#include "cedos/drivers/graphics.h"

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
        file_dir_next
    };

extern void syscall_interrupt(void);

/*!
 * Installs the syscall interrupt to INT 0x30
 */
int syscall_init(void) {
    install_interrupt(0x30, &syscall_interrupt, 0x18, INT_GATE);
    return 1;
}