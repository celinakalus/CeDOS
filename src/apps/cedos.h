#ifndef CEDOS_H
#define CEDOS_H

#include "assembly.h"

int sysprint(const char *fmt, int arg1, int arg2);
int yield();
int get_pid();

#endif