#ifndef PIPE_H
#define PIPE_H

#include "file.h"

int pipe_read(int fd, char *buffer, uint32_t size);
int pipe_write(int fd, char *buffer, uint32_t size);

extern file_operations_t pipe_fops;

#endif