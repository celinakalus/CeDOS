#ifndef CEDOS_H
#define CEDOS_H

#include "assembly.h"

#include <stdint.h>

int sysprint(const char *fmt, int arg1, int arg2);
int yield();
int get_pid();
int process_spawn(const char *fname, const char *args);
int process_spawn_pipe(const char *fname, const char *args, int stdin, int stdout);
void process_wait(int pid);

void graphics_set_mode(int mode);

int sc_file_read(int fd, char *buffer, uint32_t size);
int sc_file_write(int fd, char *buffer, uint32_t size);
int sc_file_open(char *buffer, uint32_t flags);
int sc_file_lseek(int fd, uint32_t offset, int whence);
int sc_file_tell(int fd);

void hard_reset();

int dir_next(int fd, int index, char *fname_buffer);


#endif