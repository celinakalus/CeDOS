#include "drivers/console.h"
#include "drivers/keyboard.h"
#include "drivers/graphics.h"


#include "sched/sched.h"
#include "sched/process.h"

#include "mm/paging.h"
#include "memory.h"

#include "interrupts.h"
#include "syscall.h"
#include "pic.h"
#include "pit.h"
#include "core.h"

#include "elf.h"

#include "file.h"
#include "fat.h"

#include "linker.h"
#include "assert.h"
#include "string.h"

#ifdef DEBUG
#define PRINT_DBG(...) printk("[" __FILE__ "] " __VA_ARGS__)
#else
#define PRINT_DBG(...) {}
#endif


int os_init(void) {
    core_init();
    printk("Core functions initialized.\n");

    printk("Initializing PIC...");
    pic_init();
    printk("done.\n");

    printk("Initializing PIT...");
    pit_init();
    printk("done.\n");
    
    printk("Initializing interrupts...");
    interrupts_init();
    printk("done.\n");

    printk("Setting up paging...");
    paging_init();
    printk("done.\n");

    printk("Activating interrupts...");
    sti();
    printk("done.\n");

    printk("Initiallizing malloc...");
    malloc_init(0xC0400000u, 0xC0800000u);
    printk("done.\n");

    printk("Installing syscalls...");
    syscall_init();
    printk("done.\n");

    printk("Initializing scheduler...");
    sched_init();
    printk("done.\n");

    printk("Initializing keyboard...");
    ps2_kb.init();
    printk("done.\n");

    printk("Initializing files...");
    file_init();
    printk("done.\n");

    printk("Initializing FAT file system...");
    FAT_init();
    printk("done.\n");

    printk("Initializing graphics...");
    graphics_init();
    printk("done.\n");


    printk("Initialization finished.\n--------------\n");

    return 1;
}

void infodump(void) {
    uint32_t res[4];
    cpuid(0, &res[0], &res[2], &res[1]);
    res[3] = 0;
    printk("CPU manufacturer: %s\n", res);
}

extern uint8_t* IDT;

void tasktree_r(PROCESS *p, int n) {
    if (p == NULL) { return; }

    for (int i = 0; i < n - 1; i++) {
        printk("|  ");
    }

    printk("|--%s\n", p->name);

    for (PROCESS *child = p->child; child != NULL; child = child->next_sibling) {
        for (int i = 0; i < n; i++) {
            printk("|  ");
        }
        printk("|\n");
        tasktree_r(child, n + 1);
    }
}

void tasktree(PROCESS_ID pid) {
    PROCESS *process = get_process(pid);
    if (process != NULL) {
        crit_enter();
        printk("Current task tree:\n");
        tasktree_r(process, 1);
        crit_exit();
    }
}

int os_main(void) {
    infodump();
 
    printk("Starting scheduler.\n");
    sched_start();

    printk("Creating tasks.\n");
    
    /**
     * Here, the kernel "forks" into the idle process and
     * the shell.
     * 
     * The idle process is executed whenever, duh, all
     * processes are out of work and idling.
     * 
     * The shell is where the user can give commands and
     * start her own processes. It is an executable that
     * is read from disk.
    */
    int pid = sched_spawn("shelf", "Hello World!", 0);
    assert(pid != -1);

    // here is where the idle process (PID 0) starts.
    sched_idle(NULL);

    // this should never happen.
    kpanic("Returned from idle process!");

    return 0;
}