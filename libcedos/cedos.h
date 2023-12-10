#ifndef CEDOS_H
#define CEDOS_H

#include "assembly.h"

#include <stdint.h>

typedef struct {
    int year;
    int month;
    int day;
    
    int hour;
    int minute;
    int second;
} datetime_t;

int sysprint(const char *fmt, int arg1, int arg2);
int yield();
void sleep(int ticks);
int get_pid();
int process_spawn(const char *fname, const char *args);
int process_spawn_pipe(const char *fname, const char *args, int stdin, int stdout);
void process_wait(int pid);
void process_kill(int pid);

void graphics_set_mode(int mode);

int sc_file_read(int fd, char *buffer, uint32_t size);
int sc_file_write(int fd, char *buffer, uint32_t size);
int sc_file_open(char *buffer, uint32_t flags);
int sc_file_lseek(int fd, uint32_t offset, int whence);
int sc_file_tell(int fd);

void hard_reset();

int dir_next(int fd, int index, char *fname_buffer);

int sc_time_get_ticks(void);
uint32_t sc_mem_usage(void);

int time_now(datetime_t *buffer);


#endif