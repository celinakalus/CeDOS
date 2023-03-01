#ifndef ASSEMBLY_H
#define ASSEMBLY_H


#define interrupt(num, res, arg1, arg2, arg3, arg4) \
    __asm__ volatile (  \
            "mov %1, %%eax;" \
            "mov %2, %%ebx;" \
            "mov %3, %%ecx;" \
            "mov %4, %%edx;" \
            "int $" #num ";" \
            "mov %%eax, %0;" \
            : "=m" (res) \
            : "" (arg1), "" (arg2), "" (arg3), "" (arg4) \
            : "eax", "ebx", "ecx", "edx" \
        )

/*!
 * Waits for the next interrupt.
 */
__attribute__((always_inline)) inline void hlt(void) {
    __asm__ volatile ("hlt");
}

#endif