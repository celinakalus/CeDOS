#include "cedos/drivers/console.h"
#include "cedos/drivers/keyboard.h"

#include "cedos/sched/sched.h"
#include "cedos/sched/process.h"

#include "cedos/mm/paging.h"

#include "cedos/interrupts.h"
#include "cedos/syscall.h"
#include "cedos/pic.h"
#include "cedos/pit.h"
#include "cedos/core.h"

#include "cedos/elf.h"

#include "linker.h"
#include "assert.h"

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

int sysinit(void) {
    uint8_t scancode = 0;

    memdump((VIRT_ADDR)0x10000000, 0x3000);

    //syscall(0, 0xCAFEBABE, 0xDEADBEEF, 0x42069420);

    printk("PRESS ENTER:");

    do {
        scancode = ps2_kb.read();
        printk("%c", scancode);
    } while (scancode != 0x1C);

    printk("THANKS, NOW PRESS ESC TO EXIT:");

    do {
        scancode = ps2_kb.read();
        printk("%c", scancode);
    } while (scancode != 0x01);

    syscall(1, 0, 0, 0);
    
    //while (1) {
    //    printk("x");
    //    hlt();
    //}

    //sched_exec(create_empty_page_dir(), fibonacci, "fibonacci");
    //sched_exec(create_empty_page_dir(), node, "node");
    //while (get_process_count() < 5) { sched_yield(); }
    //tasktree(1);
    printk("Terminating.\n");
}

int os_main(void) {
    infodump();

    // create test tasks
    printk("Creating tasks.\n");
    
    printk("Loading ELF executable.\n");
    elf_exec(SS_VMA + (ELF_LMA - SS_LMA), ELF_SIZE, "my_apps");
 
    printk("Starting scheduler.\n");
    sched_start();

    //kpanic("SIMULATED KERNEL PANIC");

    // won't be executed
    printk("Main procedure terminating.\n");
    return 0;
}