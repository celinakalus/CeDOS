#include "stdio.h"
#include "stdlib.h"
#include "string.h"

int main(char *args) {
    printf("Malloc test\n");

    void* a = malloc(1024);
    printf("addr a: %p\n", a);

    void* b = malloc(1024);
    printf("addr a: %p\n", b);

    printf("Copying test strings.\n");

    strcpy(a, "This is test string a.");
    strcpy(b, "Test string b is this.");

    printf("String in a: %s\n", a);
    printf("String in b: %s\n", b);

    printf("Testing realloc.\n");

    a = realloc(a, 512);
    b = realloc(b, 2048);

    printf("Value of a at addr %p: %s\n", a, a);
    printf("Value of b at addr %p: %s\n", b, b);
}