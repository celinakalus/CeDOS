#ifndef __TIME_H
#define __TIME_H

// frequency =  32768 >> (rate-1);
// rate at least 2 (~8kHz)
#define RTC_INT_RATE    3
#define RTC_FREQUENCY   (32768 >> (RTC_INT_RATE - 1))
#define RTC_MSEC(ms)    ((RTC_FREQUENCY / 1000) * ms)
#define RTC_SEC(s)      (RTC_FREQUENCY * s)

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