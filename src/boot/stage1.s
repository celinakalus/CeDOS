.section .mbr
.code16

  # disable interrupts
  cli

  # canonicalize %CS:%EIP to a known value
  ljmp $0, $start

start:
  # setup segments
  movw $0x0000, %ax
  movw %ax, %ss
  movw %ax, %ds
  movw %ax, %es
  movw %ax, %fs

  # setup stack
  movw $0x9000, %sp
  movw %sp, %bp

  # save boot drive index
  lea bootdriv_id, %si
  movb %dl, (%si)

  # select display page 0
  movb $0, %al
  movb $0x05, %ah
  int $0x10

  mov $load_bl_msg, %si
  call print

  # load rest of bootloader
  movw $0x0000, %bx   # buffer address
  movw $0x07e0, %ax
  movw %ax, %es       # buffer address (segment)
  movb $0x02, %ah     # function 0x02: read a sector
  movb $0x07, %al     # sectors to read count
  movb $0x00, %ch     # cylinder
  movb $0x02, %cl     # sector
  movb $0x00, %dh     # head
  lea bootdriv_id, %si
  movb (%si), %dl

  int $0x13
  jc error

bl_loaded:
  mov $done_msg, %si
  call print

  # read hard drive parameters
  movw $0x0000, %ax
  movw %ax, %es
  movw $0x0000, %di

  movb $0x08, %ah
  lea bootdriv_id, %si
  movb (%si), %dl
  int $0x13

  xor %ax, %ax
  movb %dl, %al
  lea num_drives, %si
  movb %al, (%si)

  xor %ax, %ax
  movb %dh, %al
  inc %ax
  lea num_heads, %si
  movw %ax, (%si)

  xor %ax, %ax
  movb %cl, %al
  shl $2, %ax
  movb %ch, %al
  inc %ax
  lea num_cylinders, %si
  movw %ax, (%si)

  xor %ax, %ax
  movb %cl, %al
  and $0x003F, %ax
  lea num_sectors, %si
  movw %ax, (%si)


  # check if A20 gate is enabled
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

  jmp error_loop


enabled:
  mov $enabled_msg, %si
  call print

  # reset bootdrive
  mov $10, %cx
reset:
  push %cx
  movw $reset_msg, %si
  call print
  movw $bootdrive_msg, %si
  call print

  movb $0x00, %ah
  int $0x13
  pop %cx
  jnc reset_end
  loop reset
  jc error
reset_end:

  movw $done_msg, %si
  call print

  # load second stage into memory starting at adress 0x10000
  #   (NOTE: this code can only be jumped to after loading the global descriptor table
  #          because it uses absolute adresses larger than 16 bit)
  #   (NOTE2: this routine only loads 0x48 sectors of the second stage into memory
  #          and is in general pretty whacky. should be replaced with sth more serious)

# arguments:
# - ax: first sector (LBA)
# - cx: last sector (LBA)
# - bx: address offset
load_sectors:
  movw $load_msg, %si
  call print

  movw $0x0000, %ax
  movw %ax, %ds

  movw $0x0008, %ax
  movw $0x0088, %cx
  movw $0x0000, %bx

load_sectors_loop:
  push %ax
  push %cx
  push %bx

  movw %bx, %di

  lea num_sectors, %si
  movw (%si), %bx
  inc %ax
  divb %bl
  movb %ah, %cl

  xor %ah, %ah
  lea num_heads, %si
  movw (%si), %bx
  divb %bl
  movb %ah, %dh
  movb %al, %ch

  lea bootdriv_id, %si
  movb (%si), %dl

  movw %di, %bx
  # movw $0x0000, %bx   # buffer address
  movw $0x1000, %ax
  movw %ax, %es       # buffer address (segment)

  movb $0x02, %ah     # function 0x02: read a sector
  movb $0x01, %al     # sectors to read count
  # dl (drive) keep as is
  int $0x13
  # jc error

  pop %bx
  pop %cx
  pop %ax
  inc %ax
  add $0x200, %bx
  cmp %ax, %cx
  jne load_sectors_loop

load_end:
  movw $0x0000, %si   # buffer address
  movw $0x1000, %ax
  movw %ax, %es
  call print

  movw $done_msg, %si
  call print

  movw $gdt_msg, %si
  call print

  # - load global descriptor table and far jump into protected mode
  lgdt (GDT_DESCRIPTOR)

  movw $done_msg, %si
  call print

  lidt (IDT_DESCRIPTOR)

  mov %cr0, %eax
  or $1, %eax
  mov %eax, %cr0

  # jmp error_loop

  ljmp $0x8, $protected


  # auxiliary functions
error:
  movw $fail_msg, %si
  call print
  
error_loop:
  jmp error_loop

  # ############################################
  # print
  #   prints out a string on the screen
  #   %si: points to the message to be printed
  # ############################################
print:
  movw $0x0000, %bx
  movb $0x0E, %ah

print_loop:
  lodsb
  or %al, %al
  jz print_end
  int $0x10
  jmp print_loop
print_end:
  ret

print_done:
  mov $done_msg, %si
  jmp print

print_fail:
  mov $fail_msg, %si
  jmp print


# this string needs to stay outside of data section
# because it is used before the data section is loaded
load_bl_msg:
  .ascii "Loading bootloader..."
  .byte 0

.section .text

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

.code32
protected:
  # setup registers with appropriate GDT values
  mov $0x10, %eax
  mov %eax, %ds
  mov %eax, %es
  mov %eax, %fs
  mov %eax, %gs
  mov %eax, %ss

  # jump to second stage code
  ljmp $0x08, $__SS_START
 

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

reset_msg: 
  .ascii "Resett"
  .byte 0

load_msg: 
  .ascii "Load"

bootdrive_msg:
  .ascii "ing bootdrive..."
  .byte 0

gdt_msg:
  .ascii "Setting GDT..."
  .byte 0

done_msg:
  .ascii "done"
  .byte 13
  .byte 10
  .byte 0

fail_msg:
  .ascii "fail"
  .byte 13
  .byte 10
  .byte 0

enabled_msg:
  .ascii "enabled"
  .byte 13
  .byte 10
  .byte 0

disabled_msg:
  .ascii "disabled"
  .byte 13
  .byte 10
  .byte 0

GDT_DESCRIPTOR:
  .word 0x23
  .int GDT

IDT_DESCRIPTOR:
  .word 0
  .int 0

.section .bss
bootdriv_id:
  .byte 0

num_drives:
  .byte 0

num_sectors:
  .word 0

num_cylinders:
  .word 0

num_heads:
  .word 0