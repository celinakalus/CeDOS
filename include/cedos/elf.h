#ifndef ELF_H
#define ELF_H

#include "cedos/sched/sched.h"
#include "cedos/sched/process.h"

/*!
 * Executes an elf file from memory
 */
PROCESS_ID elf_exec(VIRT_ADDR elf_pointer, uint32_t size, char *name);

#endif