.global sched_interrupt
/*!
 * Prehandles scheduler interrupts by switching from process- to scheduler-stack
 */
sched_interrupt:
  // preserve state of general purpose registers
  pusha

  // switch to scheduler stack
  mov %esp, %eax
  mov %ebp, %ebx
  mov $__SCHED_STACK_ADDR, %esp
  mov $__SCHED_STACK_ADDR, %ebp

  // pass process stack as arguments
  push %ebx
  push %eax
  xor %eax, %eax
  call sched_interrupt_c
  pop %eax
  pop %ebx

  // restore process stack and state of registers
  mov %eax, %esp
  mov %ebx, %ebp
  popa
  iret