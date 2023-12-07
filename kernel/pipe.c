#include "file.h"
#include "pipe.h"
#include "sched/sched.h"

file_operations_t pipe_fops = {
    NULL,           /* open */
    NULL,           /* openat */
    pipe_read,      /* read */
    pipe_write,     /* write */
    NULL,           /* dir_next */
    NULL            /* lseek */
};

#define PIPE_BUFFER_SIZE 512

uint8_t pipe_buffer[PIPE_BUFFER_SIZE];
int read_head = 0;
int write_head = 0;

int pipe_readc() {
    if (write_head == read_head) {
        return -1;
    }

    int rh_next = (read_head + 1) % PIPE_BUFFER_SIZE;
    uint8_t res = pipe_buffer[read_head];
    read_head = rh_next;
    return res;
}

int pipe_writec(uint8_t c) {
    int wh_next = (write_head + 1) % PIPE_BUFFER_SIZE;
    if ((wh_next + 1) % PIPE_BUFFER_SIZE == read_head) {
        return -1;
    }
    pipe_buffer[write_head] = c;
    write_head = wh_next;
    return c;
}

int pipe_read(int fd, char *buffer, uint32_t size) {
    int i = 0;
    while (i < size) {
        int res = pipe_readc();
        if (res == -1) { break; }
        buffer[i++] = (char)(res);
    }
    return i;
}

int pipe_write(int fd, char *buffer, uint32_t size) {
    int i = 0;
    while (i < size) {
        int res = pipe_writec(buffer[i++]);
        if (res == -1) { break; }
    }
    return i;
}