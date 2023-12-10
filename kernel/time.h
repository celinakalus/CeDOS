#ifndef __TIME_H
#define __TIME_H

typedef struct {
    int year;
    int month;
    int day;
    
    int hour;
    int minute;
    int second;
} datetime_t;

void time_tick(void);

int time_get_ticks(void);

int time_now(datetime_t *buffer);

#endif