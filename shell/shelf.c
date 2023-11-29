#include "cedos.h"
#include "stdio.h"
#include "string.h"
#include "stdlib.h"

#include <stdint.h>

struct hist_item {
    char* buffer;
    struct hist_item* next;
};

struct hist_item* hist_first = NULL;

char* read_line() {
    int i = 0;
    char c;

    char* buffer = malloc(512);
    buffer[0] = 0;

    struct hist_item* history = hist_first;
    
    while (1) {
        c = getchar();

        if (c == '\n') {
            break;
        } else if (c == 0) {
            continue;
        } else if (c == 0x08 && i <= 0) {
            continue;
        } else if (c == '^' && getchar() == '[' && getchar() == 'A') {
            if (history == NULL) { continue; }
            strcpy(buffer, history->buffer);
            while (i) { putchar(0x08); i--; }
            i = strlen(buffer);
            puts(buffer);
            history = history->next;
            continue;
        } else if (c == 0x08) {
            buffer[--i] = 0;
            putchar(c);
        } else {
            buffer[i++] = c;
            putchar(c);
        }
    }

    buffer[i] = 0;
    putchar(c);

    if (i > 0) {
        // append command history
        struct hist_item* next = malloc(sizeof(struct hist_item));
        next->buffer = buffer;
        next->next = hist_first;
        hist_first = next;
    }

    char *ret_buf = malloc(512);
    strcpy(ret_buf, buffer);

    return ret_buf;
}

void main(char *args) {
    uint32_t a = 0, b = 1, i = 0;
    printf("\n");
    printf("ShELF shell interface for CeDOS\n");
    printf("Version: " VERSION "\n");

    while (1) {
        printf("/> ");

        char* buffer = read_line(buffer, hist_first);

        if (strlen(buffer) == 0) { continue; }

        char *file = buffer;
        char *args = (char *)(0);

        int i = 0;
        while (buffer[i]) {
            if (buffer[i] == ' ') {
                buffer[i] = 0;
                args = &(buffer[i+1]);
                break;
            }

            i++;
        }

        //printf("Executing %s...\n", buffer);

        if (strcmp(file, "exit") == 0) {
            printf("Thank you for using ShELF!\n");
            break;
        } else if (strcmp(file, "history") == 0) {
            struct hist_item* item = hist_first;

            while (item) {
                printf("%s\n", item->buffer);
                item = item->next;
            }

            continue;
        }

        int pid = process_spawn(file, args);

        //printf("Child process %i spawned, waiting for termination...\n", pid);

        if (pid == -1) {
            printf("File not found: %s\n", buffer);
        } else {
            process_wait(pid);
        }

        //printf("Child process %i terminated.\n", pid);
    }
}