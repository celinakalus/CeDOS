#include "time.h"
#include "assembly.h"

#define RTC_COMMAND  			0x70
#define RTC_DATA 				0x71

#define RTC_READY   			0x80

#define RTC_REGISTER_A			0x0A
#define RTC_REGISTER_B			0x0B
#define RTC_REGISTER_C			0x0C

#define RTC_GET_SECONDS			0x00
#define RTC_GET_MINUTES			0x02
#define RTC_GET_HOURS			0x04

#define RTC_GET_DAY 			0x07
#define RTC_GET_MONTH			0x08
#define RTC_GET_YEAR			0x09

int ticks = 0;

void time_tick(void) {
    ticks++;
}

int time_get_ticks(void) {
    return ticks;
}

static uint8_t rtc_get(uint8_t reg) {
    outb(reg, RTC_COMMAND);
    nop();
    nop();
    nop();
    nop();
    uint8_t res = inb(RTC_DATA);
    return (res >> 4) * 10 + (res & 0x0F);
}

int time_now(datetime_t *buffer) {
    buffer->year = 2000 + rtc_get(RTC_GET_YEAR);
    buffer->month = rtc_get(RTC_GET_MONTH);
    buffer->day = rtc_get(RTC_GET_DAY);

    buffer->hour = rtc_get(RTC_GET_HOURS);
    buffer->minute = rtc_get(RTC_GET_MINUTES);
    buffer->second = rtc_get(RTC_GET_SECONDS);

    return 0;
}