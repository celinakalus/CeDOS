#include "drivers/console.h"
#include "linker.h"
#include "assembly.h"
#include "string.h"

#define VGA_TEXTMODE_COLUMNS 80
#define VGA_TEXTMODE_LINES 25
#define VGA_TEXTMODE_BPC 2
#define VGA_TEXTMODE_LINE_WIDTH (VGA_TEXTMODE_COLUMNS * VGA_TEXTMODE_BPC)
#define VGA_TEXTMODE_CELLS (VGA_TEXTMODE_COLUMNS * VGA_TEXTMODE_LINES)
#define VGA_MEM_POS(line, column) ((line) * VGA_TEXTMODE_COLUMNS * VGA_TEXTMODE_BPC + (column) * VGA_TEXTMODE_BPC)
#define VGA_MEM_CELL(line, column) ((line) * VGA_TEXTMODE_COLUMNS + (column))

#define VGA_TEXTMODE_MEM ((uint8_t*)0xB8000)
#define VGA_MEM_VALUE(line, column) (VGA_TEXTMODE_MEM[VGA_MEM_POS((line), (column))])
#define VGA_MEM_COLOR(line, column) (VGA_TEXTMODE_MEM[VGA_MEM_POS((line), (column)) + 1])
#define VGA_MEM_ADDR(line, column) (&(VGA_MEM_VALUE((line), (column))))

#define VGA_INDEX_REG ((uint16_t)0x3D4)
#define VGA_DATA_REG ((uint16_t)0x3D5)

uint32_t line = 0;
uint32_t column = 0;
uint8_t color = 0x0F;

int vga_con_init(void);
void vga_con_write_c(const char c);
void vga_con_write_n(const char *string, uint32_t num);
void vga_con_write_s(const char *string);
void vga_con_clear(int mode);
void vga_con_clear_line(int mode);
void vga_con_newline(void);
void vga_con_backspace(void);

CON_DRIVER vga_con = {
    "VGA console",
    vga_con_init,
    vga_con_write_c,
    vga_con_write_n,
    vga_con_write_s,
    vga_con_clear,
    vga_con_newline,
    vga_con_backspace
};

__attribute((always_inline)) inline void set_char(char value) {
    VGA_MEM_VALUE(line, column) = value;
    VGA_MEM_COLOR(line, column) = color;
}

__attribute((always_inline)) inline void lfcr() {
    line++;
    column = 0;
    
    if (line >= VGA_TEXTMODE_LINES) {
        memcpy(
                VGA_MEM_ADDR(0, 0), 
                VGA_MEM_ADDR(1, 0), 
                VGA_TEXTMODE_LINE_WIDTH * (VGA_TEXTMODE_LINES - 1)
            );
        memset(
                VGA_MEM_ADDR(VGA_TEXTMODE_LINES - 1, 0), 
                0, 
                VGA_TEXTMODE_LINE_WIDTH
            );
        line = VGA_TEXTMODE_LINES - 1;
    }
}

__attribute((always_inline)) inline void write_char(char value) {
    if (column >= VGA_TEXTMODE_COLUMNS  || value == '\n') {
        lfcr();
    }
    
    switch (value) {
        case '\n':
            break;
        case '\0':
            break;
        default:
            set_char(value);
            column++;
            break;
    }
}

__attribute((always_inline)) inline void set_cursor(uint16_t line, uint16_t column) {
    uint16_t pos = line * VGA_TEXTMODE_COLUMNS + column;
    outb(0x0F, VGA_INDEX_REG);
    outb((uint8_t)(pos & 0xFF), VGA_DATA_REG);
    outb(0x0E, VGA_INDEX_REG);
    outb((uint8_t)(pos >> 8), VGA_DATA_REG);
}

void enable_cursor(void) {
    outb(0x0A, VGA_INDEX_REG);
    outb((inb(VGA_DATA_REG) & 0x0C) | 0x00, VGA_DATA_REG);
    outb(0x0B, VGA_INDEX_REG);
    outb((inb(VGA_DATA_REG) & 0xE0) | 0x0F, VGA_DATA_REG);
}

int vga_con_init(void) {
    vga_con_clear(2);
    
    enable_cursor();
    set_cursor(line, column);
    
    for (uint32_t i = 0; i < 320 * 200; i++) {
        uint8_t* disp = (uint8_t*)0xA0000;
        disp[i] = (uint8_t)i;
    }
    
    return 1;
}

void vga_con_write_c(const char c) {
    static enum {
        NORMAL = 0,
        ESCAPE_START = 1,
        ESCAPE_N = 2,
        ESCAPE_M = 3,
        ESCAPE_C = 4
    } state;
    static int n, m;

    if (state == NORMAL && c == '\e') {
        // beginning of escape sequence
        state = ESCAPE_START;
        n = 0;
        m = 0;
    } else if (state == ESCAPE_START && c == '[') {
        state = ESCAPE_N;
    } else if (state == ESCAPE_N && c >= '0' && c <= '9') {
        n = n * 10 + (c - '0');
    } else if (state == ESCAPE_M && c >= '0' && c <= '9') {
        m = m * 10 + (c - '0');
    } else if (state == ESCAPE_N && c == ';') {
        state = ESCAPE_M;
    } else if (state == ESCAPE_N && c == 'G') {
        column = n;
        //set_cursor(line, column);
        state = NORMAL;
    } else if (state == ESCAPE_M && c == 'H') {
        line = n;
        column = m;
        //set_cursor(line, column);
        state = NORMAL;
    } else if (state == ESCAPE_N && c == 'J') {
        vga_con_clear(n);
        state = NORMAL;
    } else if (state == ESCAPE_N && c == 'K') {
        vga_con_clear_line(n);
        state = NORMAL;
    } else if (state == ESCAPE_N && c == 'm') {
        uint8_t color_compat[8] = { 0x00, 0x04, 0x02, 0x06, 0x01, 0x05, 0x03, 0x07 };
        if (n == 0) {
            color = 0x0F;
        } else if (n >= 30 && n <= 37) {
            color = (color & 0xF0) | color_compat[n - 30];
        } else if (n >= 40 && n <= 47) {
            color = (color & 0x0F) | (color_compat[n - 40] << 4);
        } else if (n >= 90 && n <= 97) {
            color = (color & 0xF0) | (color_compat[n - 90] + 8);
        } else if (n >= 100 && n <= 107) {
            color = (color & 0x0F) | ((color_compat[n - 100] + 8) << 4);
        } else if (n < 106) {
            color = n - 90 + 8;
        }
        state = NORMAL;
    } else if (state != NORMAL) {
        state = NORMAL;
    } else if (c == 0x08) {
        vga_con_backspace();
        //set_cursor(line, column);
    } else {
        write_char(c);
        //set_cursor(line, column);
    }
}

void vga_con_write_n(const char *string, uint32_t num) {
    for (uint32_t i = 0; i < num; i++) {
        vga_con_write_c(string[i]);
    }
    set_cursor(line, column);
}

void vga_con_write_s(const char *string) {
    while (*string) {
        write_char(*(string++));
    }
    set_cursor(line, column);
}

void vga_con_clear_line(int mode) {
    // clear from cursor to end of screen
    // clear from beginning of screen to cursor
    // clear whole display
    int start, end;
    
    switch(mode) {
    case 0:
        start = column;
        end = VGA_TEXTMODE_COLUMNS;
        break;
    case 1:
        start = 0;
        end = column;
        break;
    default:
    case 2:
        start = 0;
        end = VGA_TEXTMODE_COLUMNS;
        break;
    }

    for (int i = start; i < end; i++) {
        VGA_MEM_VALUE(line, i) = 0;
        VGA_MEM_COLOR(line, i) = color;
    }
}

void vga_con_clear(int mode) {
    int cursor_pos = VGA_MEM_CELL(line, column);
    int start, end;

    switch(mode) {
    case 0:
        start = cursor_pos;
        end = VGA_TEXTMODE_CELLS;
        break;
    case 1:
        start = 0;
        end = cursor_pos;
        break;
    default:
    case 2:
        start = 0;
        end = VGA_TEXTMODE_CELLS;
        break;
    }

    for (int i = 0; i < VGA_TEXTMODE_CELLS; i++) {
        VGA_TEXTMODE_MEM[2 * i] = 0;
        VGA_TEXTMODE_MEM[2 * i + 1] = color;
    }
    
    line = 0;
    column = 0;
}

void vga_con_newline(void) {
    lfcr();
}

void vga_con_backspace(void) {
    if (column == 0 && line > 0) {
        line--;
        column = VGA_TEXTMODE_COLUMNS - 1;
    } else if (column > 0) {
        column--;
    }

    set_char(0);
    set_cursor(line, column);
}