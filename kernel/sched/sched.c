#include "sched/sched.h"
#include "sched/process.h"
#include "sched/sched_strats.h"

#include "mm/paging.h"
#include "memory.h"

#include "drivers/console.h"
#include "drivers/speaker.h"

#include "core.h"
#include "interrupts.h"
#include "pit.h"
#include "pic.h"
#include "elf.h"
#include "file.h"
#include "alarm.h"
#include "time.h"

#include "assembly.h"
#include "assert.h"

#define KERNEL_PRIVATE_STACK (void*)(0xC0600000)
#define USER_STACK (void*)(0xC0000000)

#define PROCESS_STD_EFLAGS (0x00000200)

#ifdef DEBUG
#define PRINT_DBG(...) printk("[" __FILE__ "] " __VA_ARGS__)
#else
#define PRINT_DBG(...) {}
#endif

PROCESS* get_slot(void) {
    PROCESS *new_process = (PROCESS*)malloc(sizeof(PROCESS));
    memset(new_process, 0, sizeof(PROCESS));
    return new_process;
}

PROCESS_ID current_pid;

PROCESS_ID get_current_process(void) {
    return current_pid;
}

int sched_dispatcher(void);


void sched_idle(char *args) {
    while (1) { 
        hlt();
    }
}

/*!
 * Spawn a new process and returns its process ID.
 */
PROCESS_ID sched_spawn(const char *name, char *args, int flags) {
    crit_enter();

    PRINT_DBG("process name: %s\n", name);
    PRINT_DBG("process args: %s\n", args);
    PRINT_DBG("process flags: %i\n", flags);

    int fd = file_open(name, 0);
    if (fd == -1) { return -1; }

    PHYS_ADDR page_dir = create_empty_page_dir();

    // set process context
    PROCESS *p = get_slot();
    p->page_dir = page_dir;
    p->ebp = USER_STACK;
    p->esp = USER_STACK - sizeof(SCHED_FRAME);
    p->eflags = PROCESS_STD_EFLAGS;
    p->entry = (PROCESS_MAIN*)(0xDEADBEEF);
    p->eip = sched_dispatcher;

    if (flags != 0) {
        p->stdin = (int)(flags & 0xFF);
        p->stdout = (int)(flags >> 8);
    } else {
        p->stdin = 0;
        p->stdout = 1;
    }
    
    // TODO: implement with malloc
    strcpy(p->name_buf, name);

    if (args == 0) {
        p->args_buf[0] = 0;
    } else {
        strcpy(p->args_buf, args);
    }

    p->name = (const char*)&(p->name_buf);
    p->args = (const char*)&(p->args_buf);

    PROCESS_ID pid = add_process(p, current_pid);
    p->id = pid;

    // setup stack
    static SCHED_FRAME frame;
    frame.eax = frame.ebx = frame.ecx = frame.edx = 0;
    frame.esi = frame.edi = 0;
    frame.ebp = (uint32_t)(p->ebp);
    frame.esp = (uint32_t)(p->esp);
    frame.eflags = p->eflags;
    frame.cs = 0x18;
    frame.eip = (uint32_t)(p->eip);

    // load stack
    copy_to_pdir(&frame, sizeof(frame), p->page_dir, p->esp);

    // save stack checksum
    stack_compute_checksum(&(p->checksum), &frame, &(&frame)[1]);

    /* TODO: check if code exists at entry point */

    // start the process
    p->state = PSTATE_READY;
    p->starvation = 0;

    crit_exit();

    return pid;
}


void sched_interrupt_c(SCHED_FRAME * volatile frame, uint32_t volatile ebp) {
    PROCESS* current = get_process(current_pid);

    current->esp = (VIRT_ADDR)(frame);
    current->ebp = (VIRT_ADDR)(ebp);
    current->eip = (VIRT_ADDR)frame->eip;
    current->eflags = frame->eflags;

    // save stack checksum
    stack_compute_checksum(&(current->checksum), current->esp, current->ebp);

    PRINT_DBG("esp: %p\n", current->esp);
    PRINT_DBG("ebp: %p\n", current->ebp);
    PRINT_DBG("eip: %p\n", current->eip);
    PRINT_DBG("eflags: %p\n", current->eflags);

    // select next process
    PRINT_DBG("exiting %i, ", current_pid);
    current_pid = next_schedule(current_pid);
    PRINT_DBG("entering %i\n", current_pid);

    // prepare to return to process
    PROCESS* next = get_process(current_pid);
    switch_page_dir(next->page_dir);

    PRINT_DBG("esp: %p\n", next->esp);
    PRINT_DBG("ebp: %p\n", next->ebp);
    PRINT_DBG("eip: %p\n", next->eip);
    PRINT_DBG("eflags: %p\n", next->eflags);

    STACK_CHECKSUM checksum;
    stack_compute_checksum(&(checksum), next->esp, next->ebp);

    // check stack
    if (stack_compare_checksum(&(next->checksum), &(checksum))) {
        printk("STACK DAMAGED: PROCESS %i (%s), ESP %X, EBP %X\n", current_pid, get_process(current_pid)->name, next->esp, next->ebp);
        memdump((void*)(next->esp), (void*)((uint32_t)(next->ebp) - (uint32_t)(next->esp)));
        kpanic("CRITICAL STACK DAMAGE");
    }

    // prepare stack
    frame = (volatile SCHED_FRAME*)(next->esp);
    ebp = next->ebp;

    frame->cs = 0x18;
    frame->eip = next->eip;
    frame->eflags = (uint32_t)(next->eflags);
    frame->esp = (uint32_t)(next->esp);
    frame->ebp = (uint32_t)(next->ebp);

    PRINT_DBG("esp: %p, ebp: %p, eip: %p, eflags: %p\n", frame->esp, frame->ebp, frame->eip, frame->eflags);


    // reset the timer
    pit_setup_channel(PIT_CHANNEL_0, PIT_MODE_0, SCHED_INTERVAL);
    
    pic1_eoi();
}

extern void* sched_interrupt;

int sched_init(void) {
    // install scheduler interrupt
    install_interrupt(PIC1_IRQ(0x00), &sched_interrupt, 0x18, INT_GATE);

    current_pid = 0;

    // create idle process
    PROCESS *idle = get_slot();
    assert(idle != NULL);

    PROCESS_ID idle_pid = add_process(idle, NULL);
    assert(idle_pid == 0);

    idle->page_dir = get_current_page_dir();
    idle->state = PSTATE_RUNNING;

    idle->state = PSTATE_READY;

    return 1;
}

void sched_yield(void) {
    crit_enter();
    PROCESS *current = get_process(current_pid);

    uint32_t csc = crit_stash();
    INT(0x20);
    sti();
    crit_restore(csc);

    crit_exit();
}

void sched_sleep(int msec) {
    // block the process. unblocking is done by the alarm.
    PROCESS *process = get_process(current_pid);
    process->state = PSTATE_BLOCKED;

    // create a wakeup-alarm to unblock the process.
    alarm_add(RTC_MSEC(msec), current_pid, ALARM_WAKEUP);

    sched_yield();

    // if this happens, it means the scheduler chose
    // a blocked process as next process, which should
    // never happen.
    if (process->state == PSTATE_BLOCKED) {
        kpanic("ERROR!");
    }
}

void sched_unblock(int pid) {
    PROCESS *process = get_process(pid);
    
    // only unblock previously blocked processes
    if (process->state == PSTATE_BLOCKED) {
        process->state = PSTATE_READY;
    }
}

int sched_kill(PROCESS_ID pid) {
    int success = 1;
    crit_enter();
    
    PROCESS *process = get_process(pid);
    if (process != NULL) {
        // kill all children
        for (PROCESS *child = process->child; child != NULL; child = child->next_sibling) {
            sched_kill(child->id);
        }

        process->state = PSTATE_TERMINATED;
    } else {
        success = 0;
    }

    if (pid == current_pid) {
        // current process has terminated
        sched_yield();
    }


    crit_exit();
    return success;
}

void sched_wait(PROCESS_ID pid) {
    assert(pid != current_pid);

    while (1) {
        PROCESS *p = get_process(pid);
        if (p == NULL || p->state == PSTATE_TERMINATED) { break; }

        sched_yield();    
    }
}

int sched_start(void) {
    current_pid = 0;

    // perform the first timer interrupt manually
    pic_unmask_interrupt(0);
    INT(0x20);

    return 0;
}

int sched_stop(void) {
    // send signals to all processes and allow them to wrap up

    // kill all processes

    // disable interrupts
    pic_mask_interrupt(0);

    return 0;
}

int sched_dispatcher(void) {
    PRINT_DBG("Dispatching process %i...\n", current_pid);

    PROCESS* this = get_process(current_pid);


    // enter the actual program
    elf_exec(this->name, this->args);

    PRINT_DBG("Process %i terminated.\n", current_pid);

    sched_kill(current_pid);

    // just for absolute safety
    kpanic("Executing a terminated process!!\n");

    return 0;
}