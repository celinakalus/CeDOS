#include "sched/process.h"

#define NULL ((void*)0)

PROCESS_ID next_schedule(PROCESS_ID current) {
    PROCESS* next = get_first_process();

    PROCESS* choice = NULL;

    while (next) {
         // skip idle process
        if (next->id == 0) {
            next = next->next;
            continue;
        }

        next->starvation++;

        // choose ready process with highest starvation value
        if (next->state == PSTATE_READY && next->starvation > choice->starvation) {
            choice = next;
        }

        next = next->next;
    }

    if (choice) {
        choice->starvation = 0;
        return choice->id;
    } else {
        return 0;
    }
}