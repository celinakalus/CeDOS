#include "core.h"
#include "drivers/console.h"
#include "assembly.h"

CON_DRIVER *core_con;

char numeric[] = { '0', '1', '2', '3', '4', '5', '6', '7', '8', '9' };
char hex[] = { '0', '1', '2', '3', '4', '5', '6', '7', '8', '9', 'A', 'B', 'C', 'D', 'E', 'F' };

void printk_hex_char(uint8_t c) {
    core_con->write_c(hex[c >> 4]);
    core_con->write_c(hex[c & 0x0F]);
}

void printk_uint32(uint32_t value) {
    uint8_t* mem = (uint8_t*)(&value);
    for (int i = 0; i < 4; i++) {
        printk_hex_char(mem[3-i]);
    }
}

void rek_printk_uint(unsigned int value) {
    if (value > 0) {
        rek_printk_uint(value / 10);
        core_con->write_c(numeric[value % 10]);
    }
}

void printk_uint(unsigned int value) {
    if (value == 0) {
        core_con->write_c('0');
        return;
    }

    rek_printk_uint(value);
}

void printk_int(int value) {
    if (value < 0) {
        core_con->write_c('-');
        printk_int(-value);
        return;
    } else if (value == 0) {
        core_con->write_c('0');
        return;
    }

    rek_printk_uint((unsigned int)value);
}

void memdump(void* start, uint32_t size) {
    crit_enter();
    uint8_t* _start = (uint8_t*)((uint32_t)start & 0xFFFFFFF0);
    uint8_t* _end = (uint8_t*)(((uint32_t)start + size + 0xF) & 0xFFFFFFF0);

    uint32_t first_line = (uint32_t)(_start);
    uint32_t last_line = (uint32_t)(_end);

    for (uint32_t i = first_line; i < last_line; i += 0x10) {
        core_con->write_c(' ');
        printk_uint32(i);
        core_con->write_c(' ');

        for (int j = 0; j < 0x10; j++) {
            uint8_t* p = (uint8_t*)(i | j);
            
            if (p >= (uint8_t*)(start) && p < (uint8_t*)(start + size)) {
                printk_hex_char(*p);
                core_con->write_c(' ');
            } else {
                core_con->write_c(' ');
                core_con->write_c(' ');
                core_con->write_c(' ');
            }
        }
        core_con->write_c('\n');
    }

    crit_exit();
}

void stackdump(void) {
    void *esp, *ebp;
    __asm__ volatile ("mov %%esp, %0; mov %%ebp, %1" : "=m" (esp), "=m" (ebp));
    printk("STACK DUMP:\n");
    memdump(esp, (uint32_t)ebp - (uint32_t)esp);
}

void regdump(void) {
    uint32_t eax, ebx, ecx, edx,
             esi, edi, esp, ebp;
    __asm__ volatile (  "mov %%eax, %0;"
                        "mov %%ebx, %1;"
                        "mov %%ecx, %2;"
                        "mov %%edx, %3;"
                        "mov %%esi, %4;"
                        "mov %%edi, %5;"
                        "mov %%esp, %6;"
                        "mov %%ebp, %7;"
                        : "=m" (eax),
                        "=m" (ebx),
                        "=m" (ecx),
                        "=m" (edx),
                        "=m" (esi),
                        "=m" (edi),
                        "=m" (esp),
                        "=m" (ebp));

    printk(" EAX=%X EBX=%X ECX=%X EDX=%X\n", eax, ebx, ecx, edx);
    printk(" ESI=%X EDI=%X ESP=%X EBP=%X\n", esi, edi, esp, ebp);
}

void printk(const char* fmt, ...) {
    crit_enter();
    va_list args;
    va_start(args, fmt);

    enum {
        STATE_DEFAULT,
        STATE_ARGUMENT,
    } state = STATE_DEFAULT;

    while (*fmt) {
        if (state == STATE_ARGUMENT && *fmt == 'X') {
            printk_uint32(va_arg(args, uint32_t));
            state = STATE_DEFAULT;
        } else if (state == STATE_ARGUMENT && *fmt == 'x') {
            printk_uint32(va_arg(args, uint32_t));
            state = STATE_DEFAULT;
        } else if (state == STATE_ARGUMENT && *fmt == 'i') {
            printk_int(va_arg(args, int));
            state = STATE_DEFAULT;
        } else if (state == STATE_ARGUMENT && *fmt == 'u') {
            printk_uint(va_arg(args, unsigned int));
            state = STATE_DEFAULT;
        } else if (state == STATE_ARGUMENT && *fmt == 'p') {
            printk_uint32(va_arg(args, uint32_t));
            state = STATE_DEFAULT;
        } else if (state == STATE_ARGUMENT && *fmt == 's') {
            const char* string = va_arg(args, const char*);
            while (*string) { core_con->write_c(*string++); }
            state = STATE_DEFAULT;
        } else if (state == STATE_ARGUMENT && *fmt == 'c') {
            core_con->write_c(va_arg(args, int));
            state = STATE_DEFAULT;
        } else if (state == STATE_ARGUMENT && *fmt == '%') {
            core_con->write_c('%');
            state = STATE_DEFAULT;
        } else if (*fmt == '%') {
            state = STATE_ARGUMENT;
        } else {
            core_con->write_c(*fmt);
        }

        fmt++;
    }

    crit_exit();
}

void kpanic(const char* string) {
    cli();
    printk(string);
    core_con->write_c('\n');
    // register dump / stack dump
    regdump();
    stackdump();
    while (1) {}
}

void kfault(const char* string, INTERRUPT_FRAME *frame, uint16_t err_code) {
    cli();
    printk("%s\n", string);
    printk("EIP:     %p\n", frame->eip);
    printk("CS:      %p\n", frame->cs);
    printk("EFLAGS:  %p\n", frame->eflags);
    // register dump / stack dump
    regdump();
    stackdump();
    while (1) {}
}

uint32_t crit_sect_counter = 0;
uint32_t if_state = 0;

void crit_enter(void) {
    if (crit_sect_counter++ == 0) { 
        if_state = get_eflags() & (1 << 9); 
        cli(); 
    }
}

void crit_exit(void) {
    if (--crit_sect_counter == 0) {
        uint32_t eflags = get_eflags() | if_state;
        set_eflags(eflags);
    }
}

uint32_t crit_stash(void) {
    uint32_t __csc = crit_sect_counter;
    crit_sect_counter = 0;
    if (__csc > 0) {
        if_state = get_eflags() & (1 << 9); 
        sti();
    }
    return __csc;
}

void crit_restore(uint32_t state) {
    crit_sect_counter = state;
    if (crit_sect_counter > 0) {
        uint32_t eflags = get_eflags() | if_state;
        set_eflags(eflags);
    }
}

void crit_reset(void) {
    crit_sect_counter = 0;
    uint32_t eflags = get_eflags() | if_state;
    set_eflags(eflags);
}

void hard_reset(void) {
    outb(0xFE, 0x64);
}

int core_init(void) {
    core_con = std_con;
    return core_con->init();
}

void assert_failed(const char * message) {
    kpanic(message);
}
