.section .text.realmode
.global realmode_int10h
.code32
realmode_int10h:
    push %ebp
    mov %esp, %ebp

    pushf
    cli
    
    pusha

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

    mov +16(%edi), %eax
    push %eax
    mov +12(%edi), %eax
    push %eax
    mov +8(%edi), %eax
    push %eax

    push %ebp
    mov %esp, %ebp

    # disable paging
    mov %cr3, %eax
    push %eax

    mov %cr0, %eax
    and $0x3FFFFFFD, %eax
    mov %eax, %cr0

    xor %eax, %eax
    mov %eax, %cr3

    # switch to 16 bit protected mode
    ljmp $0x8, $pmode16

.code16
pmode16:
    # setup segments
    movw $0x10, %ax
    movw %ax, %ss
    movw %ax, %ds
    movw %ax, %es
    movw %ax, %fs

    # load real mode interrupt descriptor table
    sidt (pmode_IDT)
    lidt (realmode_IDT)

    # switch to real mode
    mov %cr0, %eax
    and $0xFFFFFFFE, %eax
    mov %eax, %cr0

    ljmp $0, $realmode
realmode:
    # setup segments
    movw $0x0000, %ax
    movw %ax, %ss
    movw %ax, %ds
    movw %ax, %es
    movw %ax, %fs

    sti

    # load arguments
    movw +4(%bp), %ax
    movw +8(%bp), %bx
    movw +12(%bp), %cx

    int $0x10

    cli

    lidt (pmode_IDT)

    mov %cr0, %eax
    or $0x00000001, %eax
    mov %eax, %cr0

    ljmp $0x18, $pmode32

.code32
pmode32:
    # set data segments
    movw $0x20, %ax
    movw %ax, %ss
    movw %ax, %ds
    movw %ax, %es
    movw %ax, %fs

    # restore paging
    pop %eax
    mov %eax, %cr3

    mov %cr0, %eax
    or $0x80000010, %eax
    mov %eax, %cr0

    # clean up argument buffer
    pop %ebp
    add $12, %esp

    # restore original stack
    pop %ebp
    pop %eax
    mov %eax, %esp

    popa
    popf
    
    # return
    pop %ebp
    ret


realmode_IDT:
  .word 0x3FF
  .int 0

pmode_IDT:
  .word 0
  .int 0

