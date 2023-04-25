#include "cedos/drivers/console.h"
#include "cedos/drivers/keyboard.h"
#include "cedos/drivers/graphics.h"


#include "cedos/sched/sched.h"
#include "cedos/sched/process.h"

#include "cedos/mm/paging.h"

#include "cedos/interrupts.h"
#include "cedos/syscall.h"
#include "cedos/pic.h"
#include "cedos/pit.h"
#include "cedos/core.h"

#include "cedos/elf.h"

#include "cedos/file.h"
#include "cedos/fat.h"

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

    // create test tasks
    printk("Creating tasks.\n");
    
    
    int pid = sched_spawn("shelf", "Hello World!", 0);
    assert(pid != -1);
    //sched_spawn("fibonacci.o", "Hello World!");
    //sched_spawn("fibonacci.o", "Hello World!");
 
    printk("Starting scheduler.\n");
    sched_start();

    //kpanic("SIMULATED KERNEL PANIC");

    // won't be executed
    printk("Main procedure terminating.\n");
    return 0;
}