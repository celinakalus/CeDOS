#include "cedos.h"
#include "assembly.h"
#include <stdint.h>

/*int sysprint(const char *fmt, int arg1, int arg2) {
    int res = 0;
    interrupt(0x30, res, 4, fmt, arg1, arg2);
    return res;
}*/

int yield() {
    volatile uint32_t res = 0;
    interrupt(0x20, res, 0, 0, 0, 0);
    return res;
}

int get_pid() {
    volatile uint32_t res = 0;
    interrupt(0x30, res, 3, 0, 0, 0);
    return res;
}

int file_openat(int fd, const char *fname, int flags) {
    volatile uint32_t res = 0;
    interrupt(0x30, res, 6, fd, fname, flags);
    return res;
}

int sc_file_read(int fd, char *buffer, uint32_t size) {
    volatile uint32_t res = 0;
    interrupt(0x30, res, 0, fd, buffer, size);
    return res;
}

int sc_file_write(int fd, char *buffer, uint32_t size) {
    volatile uint32_t res = 0;
    interrupt(0x30, res, 1, fd, buffer, size);
    return res;
}

int process_spawn(const char *fname, const char *args) {
    volatile uint32_t res = 0;
    interrupt(0x30, res, 4, fname, args, 0);
    return res;
}

void process_wait(int pid) {
    volatile uint32_t res = 0;
    interrupt(0x30, res, 5, pid, 0, 0);
}

int sc_file_open(char *pathname, int flags) {
    volatile uint32_t res = 0;
    interrupt(0x30, res, 6, pathname, flags, 0);
    return res;
}

void graphics_set_mode(int mode) {
    volatile uint32_t res = 0;
    interrupt(0x30, res, 7, mode, 0, 0);
}