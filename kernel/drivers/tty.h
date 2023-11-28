#ifndef TTY_H
#define TTY_H

#include "file.h"
#include "drivers/keyboard.h"
#include "drivers/console.h"

int tty_open(const char *pathname, int flags);
int tty_openat(int fd, const char *fname, int flags);
int tty_read(int fd, char *buffer, uint32_t size);
int tty_write(int fd, char *buffer, uint32_t size);

extern file_operations_t tty_fops;

#endif