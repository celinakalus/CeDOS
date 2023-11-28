#ifndef ELF_H
#define ELF_H

#include "sched/sched.h"
#include "sched/process.h"

/*!
 * Executes an elf file from memory
 */
PROCESS_ID elf_exec(const char *fname, char *args);

#endif