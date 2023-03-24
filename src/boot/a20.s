.section .text
.code16
  # check if A20 gate is enabled
.global enable_a20
enable_a20:
  mov $a20_check_msg, %si
  call print
  call checkA20
  je enabled

  # A20 is disabled, we need to enable it
  mov $disabled_msg, %si
  call print

  mov $a20_enable_msg, %si
  call print

  # A20 BIOS method
a20_bios:
  mov $a20_enable_bios_msg, %si
  call print

  movw $0x2401, %ax
  int $0x15

  mov $0x0001, %cx
  call delay

  call checkA20
  je enabled

  mov $fail_msg, %si
  call print

  # A20 keyboard controller method
a20_keyboard:
  mov $a20_enable_kb_msg, %si
  call print

  # TODO: implement

  mov $0x0001, %cx
  call delay

  call checkA20
  je enabled

  mov $fail_msg, %si
  call print

  # fast A20 method
a20_fasta20:
  mov $a20_enable_fasta20_msg, %si
  call print

  inb $0x92, %al
  orb $0x02, %al
  andb $0xFE, %al
  outb %al, $0x92

  mov $0x1000, %cx
  call delay

  call checkA20
  je enabled

  mov $fail_msg, %si
  call print

  mov $a20_fail, %si
  call print

  jmp error


enabled:
  mov $enabled_msg, %si
  call print

  ret


  # ############################################
  # checkA20
  #   checks if A20 line is enabled
  #   equal flag: * clear, if A20 is enabled
  #               * set, if A20 is disabled
  # ############################################
checkA20:
  push %ds
  push %es
  push %si
  push %di

  mov $0x0000, %ax
  mov %ax, %ds
  mov $0xFFFF, %ax
  mov %ax, %es

  mov $0x0500, %si
  mov $0x0510, %di

  movw %ds:(%si), %cx
  movw %es:(%di), %dx
  cmp %cx, %dx

  pop %di
  pop %si
  pop %es
  pop %ds
  ret


  # delays for at least a given number of cycles.
  # CX: number of cycles to delay
delay:
  nop
  loop delay
  ret


.section .data
a20_check_msg:
  .ascii "Checking A20..."
  .byte 0

a20_enable_msg:
  .ascii "Enabling A20 (BIOS)..."
  .byte 13
  .byte 10
  .byte 0

a20_fail:
  .ascii "Could not enable A20 gate. HALT"
  .byte 13
  .byte 10
  .byte 0

a20_enable_bios_msg:
  .ascii " BIOS: "
  .byte 0

a20_enable_kb_msg:
  .ascii " keyboard controller: "
  .byte 0

a20_enable_fasta20_msg:
  .ascii " fast A20 method: "
  .byte 0