#include "assembly.h"

#include <stdint.h>

extern void fib(void);

int start(char *args) {
    //uint32_t eip, esp, ebp;
    //__asm__ volatile ("call jump; jump: pop %0; mov %%esp, %1; mov %%ebp, %2" : "=m" (eip), "=m" (esp), "=m" (ebp));

    int pid = get_pid();
    while (1) {
        printf("Process #%i says: \"%s\".\n", pid, args);
        yield();
    }

    fib();

    while(1);
}