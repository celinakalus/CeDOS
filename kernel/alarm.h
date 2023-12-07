#ifndef ALARM_H
#define ALARM_H

#include "sched/process.h"

typedef enum {
    ALARM_UNDEF = 0,
    ALARM_WAKEUP = 1,
    ALARM_KILL = 2
} alarm_mode;

struct alarm {
    int alarm_id;
    int ticks_remaining;
    PROCESS_ID pid;
    alarm_mode m;

    struct alarm *next;
};

/**
 * @brief Decrease all alarm counters and act on finished alarms.
 * 
 * This function is called by sched_interrupt_c.
 */
void alarm_tick(void);

/**
 * @brief Add a new alarm.
 * 
 * @param ticks Number of ticks to count down.
 * @param pid Process ID of the process targeted by the alarm.
 * @param m Alarm mode. Defines which action to take once the timer has run out.
 * 
 * @return ID of the created alarm.
 */
int alarm_add(int ticks, PROCESS_ID pid, alarm_mode m);

/**
 * @brief Cancel an existing alarm.
 * 
 * @param alarm_id ID of the alarm to be cancelled.
 * 
 * @return 0 on success, -1 if the alarm was not found.
 */
int alarm_cancel(int alarm_id);

#endif