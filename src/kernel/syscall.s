.global syscall_interrupt
/*!
 * Prehandles scheduler interrupts by switching from process- to scheduler-stack
 */
syscall_interrupt:
  // push arguments on stack
  push %edx
  push %ecx
  push %ebx

  mov $SYSCALL_TABLE, %ecx
  mov (%ecx, %eax, 4), %eax

  call *%eax

  mov %eax, (%esi)

  // restore stack
  add $12, %esp
  iret