#include "cedos.h"
#include "assembly.h"

int sysprint(const char *fmt, int arg1, int arg2) {
    int res = 0;
    interrupt(0x30, res, 4, fmt, arg1, arg2);
    return res;
}

int yield() {
    int res = 0;
    interrupt(0x20, res, 0, 0, 0, 0);
    return res;
}

int get_pid() {
    int res = 0;
    interrupt(0x30, res, 3, 0, 0, 0);
    return res;
}