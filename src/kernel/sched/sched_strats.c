#include "cedos/sched/process.h"

#define NULL ((void*)0)

PROCESS_ID next_schedule(PROCESS_ID current) {
    PROCESS* process = get_process(current);

    if (process != NULL && process->next != NULL && process->next->state != PSTATE_TERMINATED) {
        return process->next->id;
    } else {
        PROCESS *first = get_first_process();
        return first->id;
    }
}