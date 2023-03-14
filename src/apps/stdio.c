#include "cedos.h"
#include "stdio.h"

#include <stdarg.h>

char user_numeric[] = { '0', '1', '2', '3', '4', '5', '6', '7', '8', '9' };
char user_hex[] = { '0', '1', '2', '3', '4', '5', '6', '7', '8', '9', 'A', 'B', 'C', 'D', 'E', 'F' };


int print_char(char c) {
    sc_file_write(0, &c, 1);
}

char read_char() {
    char c;
    sc_file_read(1, &c, 1);
    return c;
}

void print_hex_char(uint8_t c) {
    print_char(user_hex[c >> 4]);
    print_char(user_hex[c & 0x0F]);
}

void print_uint32(uint32_t value) {
    uint8_t* mem = (uint8_t*)(&value);
    for (int i = 0; i < 4; i++) {
        print_hex_char(mem[3-i]);
    }
}

void rek_print_uint(unsigned int value) {
    if (value > 0) {
        rek_print_uint(value / 10);
        print_char(user_numeric[value % 10]);
    }
}

void print_uint(unsigned int value) {
    if (value == 0) {
        print_char('0');
        return;
    }

    rek_print_uint(value);
}

void print_int(int value) {
    if (value < 0) {
        print_char('-');
        print_int(-value);
        return;
    } else if (value == 0) {
        print_char('0');
        return;
    }

    rek_print_uint((unsigned int)value);
}

int printf(const char *fmt, ...) {
    va_list args;
    va_start(args, fmt);
    uint32_t index = 0;

    enum {
        STATE_DEFAULT,
        STATE_ARGUMENT,
    } state = STATE_DEFAULT;

    while (*fmt) {
        if (state == STATE_ARGUMENT && *fmt == 'X') {
            print_uint32(va_arg(args, uint32_t));
            state = STATE_DEFAULT;
        } else if (state == STATE_ARGUMENT && *fmt == 'i') {
            print_int(va_arg(args, int));
            state = STATE_DEFAULT;
        } else if (state == STATE_ARGUMENT && *fmt == 'u') {
            print_uint(va_arg(args, unsigned int));
            state = STATE_DEFAULT;
        } else if (state == STATE_ARGUMENT && *fmt == 'p') {
            print_uint32(va_arg(args, uint32_t));
            state = STATE_DEFAULT;
        } else if (state == STATE_ARGUMENT && *fmt == 's') {
            const char* string = va_arg(args, const char*);
            while (*string) { print_char(*string++); }
            state = STATE_DEFAULT;
        } else if (state == STATE_ARGUMENT && *fmt == 'c') {
            print_char(va_arg(args, int));
            state = STATE_DEFAULT;
        } else if (state == STATE_ARGUMENT && *fmt == '%') {
            print_char('%');
            state = STATE_DEFAULT;
        } else if (*fmt == '%') {
            state = STATE_ARGUMENT;
        } else {
            print_char(*fmt);
        }

        fmt++;
    }
}