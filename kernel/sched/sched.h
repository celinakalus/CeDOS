/*! \file
 * Manages the distribution of processing time among processes.
 */
#ifndef SCHEDULER_H
#define SCHEDULER_H

#include <stdint.h>

#include "sched/process.h"

#include "mm/paging.h"

// 11928 ~ 10ms per interval
#define SCHED_INTERVAL (1193)

/*!
 * Structure of the process stack when the scheduler is executed.
 */
typedef struct {
    uint32_t edi; 
    uint32_t esi; 
    uint32_t ebp; 
    uint32_t esp; 
    uint32_t edx;
    uint32_t ecx; 
    uint32_t ebx; 
    uint32_t eax; 
    uint32_t eip;
    uint32_t cs; 
    uint32_t eflags;
}__attribute__((packed)) SCHED_FRAME;

/*!
 * Spawns a new process, loads it from the given ELF file, and returns its process ID.
 */
PROCESS_ID sched_spawn(const char *name, char *args, int flags);

/*!
 * Return the ID of the current process.
 * \return ID of current process.
 */
PROCESS_ID get_current_process(void);

/*!
 * Initializes the scheduler.
 */
int sched_init(void);

/*!
 * Starts the scheduler.
 * \return 1 on success, 0 on failure.
 */
int sched_start(void);

/*!
 * Stops the scheduler.
 * \return 1 on success, 0 on failure.
 */
int sched_stop(void);

/*!
 * Returns processing time to the scheduler prematurely and blocks the 
 * process for one unit of time.
 */
void sched_yield(void);

/**
 * @brief Blocks the current process for the given number of ticks.
 * 
 * @param ticks Number of ticks.
 */
void sched_sleep(int ticks);

/**
 * @brief Unblocks the process given by \p pid immediately.
 * 
 * @param pid ID of the process to be unblocked.
 */
void sched_unblock(int pid);

/*!
 * Kills the process with the specified ID.
 * \param pid Process ID of the process to be killed.
 * \return 1 on success, 0 on failure.
 */
int sched_kill(PROCESS_ID pid);

/*!
 * Wait for a process to terminate.
 * \param pid Process ID of the process to wait for.
 */
void sched_wait(PROCESS_ID pid);

/*!
 * The scheduler.
 */
void sched_interrupt_c(SCHED_FRAME * volatile frame, uint32_t volatile ebp);
#endif