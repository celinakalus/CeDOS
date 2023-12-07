#include "memory.h"
#include "alarm.h"
#include "core.h"
#include "sched/sched.h"

struct alarm *first = NULL;
int alarm_id_next = 1000;

int alarm_add(int ticks, PROCESS_ID pid, alarm_mode m) {
    struct alarm *next = malloc(sizeof(struct alarm));

    next->alarm_id = alarm_id_next++;
    next->ticks_remaining = ticks;
    next->pid = pid;
    next->m = m;

    // add to list
    next->next = first;
    first = next;

    return next->alarm_id;
}

void alarm_tick(void) {
    struct alarm **next = &first;

    while (*next) {
        if ((*next)->ticks_remaining-- <= 0) {
            // send signal to process
            switch((*next)->m) {
            case ALARM_WAKEUP:
                sched_unblock((*next)->pid);
                break;
            case ALARM_KILL:
                sched_kill((*next)->pid);
                break;
            default:
                printk("Unknown alarm mode: %i\n", (int)((*next)->m));
                break;
            }

            // remove alarm
            *next = (*next)->next;
        } else {
            next = &((*next)->next);
        }
    }
}

int alarm_cancel(int alarm_id) {
    struct alarm **next = &first;

    while (*next) {
        if ((*next)->alarm_id == alarm_id) {
            // remove alarm
            *next = (*next)->next;
            return 0;
        } else {
            next = &((*next)->next);
        }
    }

    // alarm was not found
    return -1;
}