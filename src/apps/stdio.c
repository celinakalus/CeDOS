#include "cedos.h"
#include "stdio.h"

#include <stdarg.h>
#include "string.h"

char user_numeric[] = { '0', '1', '2', '3', '4', '5', '6', '7', '8', '9' };
char user_hex[] = { '0', '1', '2', '3', '4', '5', '6', '7', '8', '9', 'A', 'B', 'C', 'D', 'E', 'F' };


int print(char *buffer, int num) {
    return sc_file_write(0, buffer, num);
}

int read_char(char *buffer, int num) {
    return sc_file_read(1, buffer, num);
}

/*void print_hex_char(uint8_t c) {
    print_char(user_hex[c >> 4]);
    print_char(user_hex[c & 0x0F]);
}*/

int sprint_hex_char(uint8_t c, char *buffer) {
    buffer[0] = user_hex[c >> 4];
    buffer[1] = user_hex[c & 0x0F];
    return 2;
}

int sprint_uint32(uint32_t value, char *buffer) {
    uint8_t* mem = (uint8_t*)(&value);
    for (int i = 0; i < 4; i++) {
        sprint_hex_char(mem[3-i], buffer);
        buffer += 2;
    }
}

int rek_sprint_uint(unsigned int value, char *buffer) {
    if (value > 0) {
        int i = rek_sprint_uint(value / 10, buffer);
        buffer[i] = user_numeric[value % 10];
        return i + 1;
    } else {
        return 0;
    }
}

int sprint_uint(unsigned int value, char *buffer) {
    if (value == 0) {
        buffer[0] = '0';
        return 1;
    } else {
        return rek_sprint_uint(value, buffer++);
    }
}

int sprint_int(int value, char *buffer) {
    if (value < 0) {
        buffer[0] = '-';
        return sprint_int(-value, buffer++) + 1;
    } else if (value == 0) {
        buffer[0] = '0';
        return 1;
    } else {
        return rek_sprint_uint((unsigned int)value, buffer);
    }
}

/*void rek_print_uint(unsigned int value) {
    if (value > 0) {
        rek_print_uint(value / 10);
        print_char(user_numeric[value % 10]);
    }
}*/

void print_uint(unsigned int value) {
    char buffer[16];
    int size = sprint_uint(value, buffer);
    print(buffer, size);
}

void print_int(int value) {
    char buffer[16];
    int size = sprint_int(value, buffer);
    print(buffer, size);
}

void print_uint32(uint32_t value) {
    char buffer[16];
    int size = sprint_uint32(value, buffer);
    print(buffer, size);
}

void print_string(char *str) {
    int length = 0;
    while (str[length]) { length++; }
    print(str, length);
}

void print_char(char c) {
    print(&c, 1);
}

int printf(const char *fmt, ...) {
    va_list args;
    va_start(args, fmt);
    uint32_t index = 0;

    while (*fmt) {
        int i = 0;
        while (fmt[i] != '\0' && fmt[i] != '%') { i++; }
        if (i > 0) { print(fmt, i); }
        if (fmt[i] == '\0') { break; }
        fmt = fmt + i + 1;

        if (*fmt == 'X') {
            print_uint32(va_arg(args, uint32_t));
        } else if (*fmt == 'i') {
            print_int(va_arg(args, int));
        } else if (*fmt == 'u') {
            print_uint(va_arg(args, unsigned int));
        } else if (*fmt == 'p') {
            print_uint32(va_arg(args, uint32_t));
        } else if (*fmt == 's') {
            const char* string = va_arg(args, const char*);
            print_string(string);
        } else if (*fmt == 'c') {
            print_char(va_arg(args, int));
        } else if (*fmt == '%') {
            print_char('%');
        } else {
            print_char(*fmt);
        }

        fmt++;
    }
}