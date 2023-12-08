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
  movw $0x7c00, %sp
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
  movw $0x0100, %ax
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

  ljmp $0, $bl_loaded

  # auxiliary functions
.global error
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
.global print
print:
  pusha
  movw $0x0000, %bx
  movb $0x0E, %ah

print_loop:
  lodsb
  or %al, %al
  jz print_end
  int $0x10
  jmp print_loop
print_end:
  popa
  ret

print_done:
  mov $done_msg, %si
  jmp print

print_fail:
  mov $fail_msg, %si
  jmp print

.global print_hex_int16
print_hex_int16:
  xchg %al, %ah
  call print_hex_char

  xchg %al, %ah
  call print_hex_char
  ret

# arguments:
# - ax: values to print
# - cx: number of values
print_hex_char:
  ror $4, %al
  call print_hex_val

  ror $4, %al
  call print_hex_val
  ret

print_hex_val:
  pusha
  movw $0x0000, %bx
  lea hex_table, %di
  
  andw $0x000F, %ax
  add %ax, %di
  movb %ds:(%di), %al
  movb $0x0E, %ah
  int $0x10
  popa
  ret


hex_table:
  .ascii "0123456789ABCDEF"

# this string needs to stay outside of data section
# because it is used before the data section is loaded
load_bl_msg:
  .ascii "Loading bootloader..."
  .byte 0

.section .text
.code16
bl_loaded:
  mov $done_msg, %si
  call print

  call read_drive_params

  call enable_a20

  call reset_drive

  call find_bootable_part

  call load_sectors

  movw $gdt_msg, %si
  call print

  # - load global descriptor table and far jump into protected mode
  lgdt (GDT_DESCRIPTOR)

  movw $done_msg, %si
  call print

  lidt (IDT_DESCRIPTOR)

  mov %cr0, %eax
  or $0x00000001, %eax
  mov %eax, %cr0

  # perform long jump to set code segment
  ljmp $0x18, $protected


myhexdump:
  movb (%si), %al
  call print_hex_char
  inc %si
  push %si
  movw $space_msg, %si
  call print
  pop %si
  loop myhexdump
  movw $newline_msg, %si
  call print
  ret


.code32
protected:
  # setup registers with appropriate GDT values
  mov $0x20, %eax
  mov %eax, %ss
  mov %eax, %ds
  mov %eax, %es
  mov %eax, %fs
  mov %eax, %gs

  call load_kernel

  test %eax, %eax
  jnz loop
  
  # create a page directory for the kernel that maps it to 0xC0000000
  # and identity maps the first 1M of memory
  call create_kernel_environment
  
  # load kernel page directory
  movl %eax, %cr3

  # set paging enable and protection bit
  movl %cr0, %eax
  or $0x80000010, %eax
  movl %eax, %cr0

  # jump to kernel code
  ljmp $0x18, $0xC0000000

  # loop until the heat death of the universe
loop:
  jmp loop
 

.section .data 
space_msg:
  .ascii " "
  .byte 0

newline_msg:
  .byte 13
  .byte 10
  .byte 0

gdt_msg:
  .ascii "Setting GDT..."
  .byte 0

.global done_msg
done_msg:
  .ascii "done"
  .byte 13
  .byte 10
  .byte 0

.global fail_msg
fail_msg:
  .ascii "fail"
  .byte 13
  .byte 10
  .byte 0

.global enabled_msg
enabled_msg:
  .ascii "enabled"
  .byte 13
  .byte 10
  .byte 0

.global disabled_msg
disabled_msg:
  .ascii "disabled"
  .byte 13
  .byte 10
  .byte 0

GDT_DESCRIPTOR:
  .word 0x37
  .int GDT

IDT_DESCRIPTOR:
  .word 0
  .int 0
