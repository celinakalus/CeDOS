#include "cedos.h"
#include "stdio.h"

#include <stdarg.h>
#include "string.h"

char user_numeric[] = { '0', '1', '2', '3', '4', '5', '6', '7', '8', '9' };
char user_hex[] = { '0', '1', '2', '3', '4', '5', '6', '7', '8', '9', 'A', 'B', 'C', 'D', 'E', 'F' };

FILE* fopen(const char* filename, const char* mode) {
    return (FILE*)(sc_file_open(filename, NULL));
}

size_t fread(void* buffer, size_t size, size_t count, FILE* file) {
    return sc_file_read((int)(file), buffer, size * count);
}

size_t fwrite(const void* buffer, size_t size, size_t count, FILE* file) {
    return sc_file_write((int)(file), buffer, size * count);
}

int fputc ( int character, FILE * stream ) {
    int retval;
    char c = character;
    retval = fwrite(&c, 1, 1, stream);

    if (retval == 1) {
        return (int)(c);
    } else {
        return EOF;
    }
}

int fgetc ( FILE * stream ) {
    int retval;
    char c = 0;
    retval = fread(&c, 1, 1, stream);

    if (retval == 1) {
        return (int)(c);
    } else {
        return EOF;
    }
}

int fputs ( const char * str, FILE * stream ) {
    int i = 0;

    while (str[i] != 0 && str[i] != '\n') { i++; }

    fwrite(str, 1, i, stream);

    return i;
}

char * fgets ( char * str, int num, FILE * stream ) {
    int i = 0;

    while (i < num) { 
        str[i] = fgetc(stream);
        if (str[i] == '\n' || str[i] == EOF) { break; }
        i++; 
    }

    return i + 1;
}

int sprintf ( char * str, const char * format, ... );
int fprintf(FILE*, const char*, ...);
int printf(const char *fmt, ...);

int fseek(FILE*, long, int);
long ftell(FILE*);


int print(char *buffer, int num) {
    return fwrite(buffer, sizeof(char), num, stdout);
}

int read_char(char *buffer, int num) {
    return sc_file_read(1, buffer, num);
}

void print_char(char c) {
    print(&c, 1);
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

int print_hex_char(uint8_t c) {
    char buffer[2];
    int size = sprint_hex_char(c, buffer);
    print(buffer, size);
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
        } else if (*fmt == 'x') {
            print_int(va_arg(args, char));
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

int fprintf(FILE*, const char*, ...);
FILE* fopen(const char*, const char*);

int fseek(FILE*, long, int);
long ftell(FILE*);

size_t fread(void*, size_t, size_t, FILE*);
size_t fwrite(const void*, size_t, size_t, FILE*);