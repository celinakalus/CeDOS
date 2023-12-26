#include "time.h"
#include "assembly.h"
#include "interrupts.h"
#include "pic.h"
#include "core.h"
#include "alarm.h"

#define RTC_COMMAND  			0x70
#define RTC_DATA 				0x71

#define RTC_READY   			0x80

#define RTC_REGISTER_A			0x0A
#define RTC_REGISTER_B			0x0B
#define RTC_REGISTER_C			0x0C
#define RTC_REGISTER_D			0x0D

#define RTC_GET_SECONDS			0x00
#define RTC_GET_MINUTES			0x02
#define RTC_GET_HOURS			0x04

#define RTC_GET_DAY 			0x07
#define RTC_GET_MONTH			0x08
#define RTC_GET_YEAR			0x09

#define RTC_DISABLE_NMI         0x80
#define RTC_ENABLE_NMI          0x00

#define RTC_ENABLE_INT8         0x40

int ticks = 0;

void time_tick(void) {
    ticks++;
}

int time_get_ticks(void) {
    return ticks;
}

static uint8_t rtc_get(uint8_t reg) {
    outb(reg, RTC_COMMAND);
    /*nop();
    nop();
    nop();
    nop();*/
    uint8_t res = inb(RTC_DATA);
    return res;
}

static uint8_t from_BCD(uint8_t bcd_value) {
    return (bcd_value >> 4) * 10 + (bcd_value & 0x0F);
}

static uint8_t rtc_set(uint8_t reg, uint8_t value) {
    outb(reg, RTC_COMMAND);
    /*nop();
    nop();
    nop();
    nop();*/
    outb(value, RTC_DATA);
}

int time_now(datetime_t *buffer) {
    buffer->year = 2000 + from_BCD(rtc_get(RTC_GET_YEAR));
    buffer->month = from_BCD(rtc_get(RTC_GET_MONTH));
    buffer->day = from_BCD(rtc_get(RTC_GET_DAY));

    buffer->hour = from_BCD(rtc_get(RTC_GET_HOURS));
    buffer->minute = from_BCD(rtc_get(RTC_GET_MINUTES));
    buffer->second = from_BCD(rtc_get(RTC_GET_SECONDS));

    return 0;
}

INTERRUPT(rtc_interrupt, frame) {
    time_tick();
    alarm_tick();

    rtc_get(RTC_REGISTER_C);

    pic2_eoi();
}

int time_init(void) {
    install_interrupt(PIC2_IRQ(0x00), rtc_interrupt, 0x18, INT_GATE);

    // disable all interrupts
    cli();

    uint8_t reg_a = rtc_get(RTC_REGISTER_A | RTC_DISABLE_NMI);
    rtc_set(RTC_REGISTER_A | RTC_DISABLE_NMI, (reg_a & 0xF0) | RTC_INT_RATE);

    uint8_t reg_b = rtc_get(RTC_REGISTER_B | RTC_DISABLE_NMI);
    rtc_set(RTC_REGISTER_B | RTC_DISABLE_NMI, reg_b | RTC_ENABLE_INT8);

    reg_b = rtc_get(RTC_REGISTER_B);

    rtc_get(RTC_REGISTER_C);
    rtc_get(RTC_REGISTER_D);
    // enable interrupts again
    sti();

    pic_unmask_interrupt(8);
}
