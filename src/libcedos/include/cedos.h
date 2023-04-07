#ifndef CEDOS_H
#define CEDOS_H

#include "assembly.h"

#include <stdint.h>

int sysprint(const char *fmt, int arg1, int arg2);
int yield();
int get_pid();
int process_spawn(const char *fname, const char *args);
void process_wait(int pid);

void graphics_set_mode(int mode);

int sc_file_read(int fd, char *buffer, uint32_t size);
int sc_file_write(int fd, char *buffer, uint32_t size);
int sc_file_openat(int fd, char *buffer, uint32_t size);

void hard_reset();


#endif