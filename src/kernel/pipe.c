#include "cedos/file.h"
#include "cedos/pipe.h"
#include "cedos/sched/sched.h"

file_operations_t pipe_fops = {
    NULL,           /* open */
    NULL,           /* openat */
    pipe_read,      /* read */
    pipe_write,     /* write */
    NULL            /* dir_next */
};

#define PIPE_BUFFER_SIZE 512

uint8_t pipe_buffer[PIPE_BUFFER_SIZE];
int read_head = 0;
int write_head = 0;

uint8_t pipe_readc() {
    while (write_head == read_head) {
        sched_yield();
    }

    int rh_next = (read_head + 1) % PIPE_BUFFER_SIZE;
    uint8_t res = pipe_buffer[read_head];
    read_head = rh_next;
    return res;
}

void pipe_writec(uint8_t c) {
    int wh_next = (write_head + 1) % PIPE_BUFFER_SIZE;
    while (wh_next == read_head) {
        sched_yield();
    }
    pipe_buffer[write_head] = c;
    write_head = wh_next;
}

int pipe_read(int fd, char *buffer, uint32_t size) {
    for (int i = 0; i < size; i++) {
        buffer[i] = pipe_readc();
    }
}

int pipe_write(int fd, char *buffer, uint32_t size) {
    for (int i = 0; i < size; i++) {
        pipe_writec(buffer[i]);
    }
}