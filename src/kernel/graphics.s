.section .text.realmode
realmode_int10h:
    push %ebp
    mov %esp, %ebp
    
    pusha

    push %ds
    push %es
    push %fs
    push %ss

    mov %esp, %eax
    mov %eax, %esi

    mov %ebp, %eax
    mov %eax, %edi

    # setup real mode stack
    mov $0xF000, %eax
    mov %eax, %esp
    mov %eax, %ebp

    push %esi
    push %edi

    # switch to realmode temporarily
    mov %cr0, %eax
    and $0xFFFFFFFE, %eax
    mov %eax, %cr0

    # perform long jump to set code segment
    ljmp $0, $realmode
return_pmode:
    # set data segments
    movw $0x10, %ax
    movw %ax, %ss
    movw %ax, %ds
    movw %ax, %es
    movw %ax, %fs

    # restore stack
    pop %ebp
    pop %eax
    mov %eax, %esp

    # restore original segments and registers
    pop %ss
    pop %fs
    pop %es
    pop %ds

    popa
    
    # return
    pop %ebp


.code16
realmode:
    # setup real mode segments
    movw $0x0000, %ax
    movw %ax, %ss
    movw %ax, %ds
    movw %ax, %es
    movw %ax, %fs

    # perform the actual interrupt

    mov %cr0, %eax
    and $0x00000001, %eax
    mov %eax, %cr0

    ljmp $0x8, $return_pmode