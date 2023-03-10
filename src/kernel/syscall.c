#include "cedos/interrupts.h"
#include "cedos/core.h"
#include "cedos/sched/sched.h"

void test(uint32_t ebx, uint32_t ecx, uint32_t edx) {
    printk("SYSCALL 0x01: EBX=%i ECX=%X EDX=%X\n", ebx, ecx, edx);
}

int __sysprint(const char *fmt, int arg1, int arg2) {
    printk(fmt, arg1, arg2);
}

void* SYSCALL_TABLE[] = { 
        test, 
        hard_reset, 
        sched_yield, 
        get_current_process,
        __sysprint
    };

extern void syscall_interrupt(void);

/*!
 * Installs the syscall interrupt to INT 0x30
 */
int syscall_init(void) {
    install_interrupt(0x30, &syscall_interrupt, 0x08, INT_GATE);
    return 1;
}