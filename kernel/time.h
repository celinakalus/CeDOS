#ifndef __TIME_H
#define __TIME_H

// frequency =  32768 >> (rate-1);
// rate at least 2 (~8kHz)
#define RTC_INT_RATE 2

typedef struct {
    int year;
    int month;
    int day;
    
    int hour;
    int minute;
    int second;
} datetime_t;

int time_init(void);

void time_tick(void);

int time_get_ticks(void);

int time_now(datetime_t *buffer);

#endif