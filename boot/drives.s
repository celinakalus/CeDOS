.section .text
.code16
# read hard drive parameters
.global read_drive_params
read_drive_params:
  mov $drive_params_msg, %si
  call print

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

  mov $done_msg, %si
  call print

  # print debug information
  mov $num_drives_msg, %si
  call print
  lea num_drives, %si
  xorw %ax, %ax
  movb (%si), %al
  call print_hex_int16
  mov $newline, %si
  call print

  mov $bootdriv_id_msg, %si
  call print
  lea bootdriv_id, %si
  xorw %ax, %ax
  movb (%si), %al
  call print_hex_int16
  mov $newline, %si
  call print

  mov $num_heads_msg, %si
  call print
  lea num_heads, %si
  xorw %ax, %ax
  movw (%si), %ax
  call print_hex_int16
  mov $newline, %si
  call print

  mov $num_cylinders_msg, %si
  call print
  lea num_cylinders, %si
  xorw %ax, %ax
  movw (%si), %ax
  call print_hex_int16
  mov $newline, %si
  call print

  mov $num_sectors_msg, %si
  call print
  lea num_sectors, %si
  xorw %ax, %ax
  movw (%si), %ax
  call print_hex_int16
  mov $newline, %si
  call print

  ret


  # reset bootdrive
.global reset_drive
reset_drive:
  mov $10, %cx
reset_drive_loop:
  push %cx
  movw $reset_msg, %si
  call print

  lea bootdriv_id, %si
  movb (%si), %dl

  movb $0x00, %ah
  int $0x13
  pop %cx
  jnc reset_end
  loop reset_drive_loop
  jc error
reset_end:

  movw $done_msg, %si
  call print

  ret


  # load second stage into memory starting at adress 0x10000
  #   (NOTE: this code can only be jumped to after loading the global descriptor table
  #          because it uses absolute adresses larger than 16 bit)
  #   (NOTE2: this routine only loads 0x48 sectors of the second stage into memory
  #          and is in general pretty whacky. should be replaced with sth more serious)

# arguments:
# - ax: first sector (LBA)
# - cx: last sector (LBA)
# - bx: address offset
.global load_sectors
load_sectors:
  movw $load_msg, %si
  call print

  movw $0x1000, %bx
  movw %bx, %es

load_sectors_loop:
  push %ax
  push %cx
  push %bx

  # call print_hex_int16

  movw %bx, %di

  # divide LBA by number of sectors
  lea num_sectors, %si
  xor %dx, %dx
  movw (%si), %bx
  divw %bx

  andw $0x003F, %dx
  movb %dl, %cl
  incb %cl

  xor %dx, %dx
  lea num_heads, %si
  movw (%si), %bx
  divw %bx
  movb %al, %ch
  movb %ah, %al
  xorb %ah, %ah
  shl $6, %al
  orb %al, %cl
  movb %dl, %dh

  lea bootdriv_id, %si
  movb (%si), %dl

  movw $0x0000, %bx
  
  # movw $load_debug_arrow, %si
  # call print

  movb $0x02, %ah     # function 0x02: read a sector
  movb $0x01, %al     # sectors to read count
  # dl (drive) keep as is

  pusha
  int $0x13
  jc load_error

  movw $load_msg_dot, %si
  call print

  popa

  pop %bx
  pop %cx
  pop %ax
  inc %ax
  movw %es, %bx
  add $0x20, %bx
  movw %bx, %es
  cmp %ax, %cx
  jne load_sectors_loop

load_end:
  movw $0x0000, %si   # buffer address
  movw $0x1000, %ax
  movw %ax, %es
  ;call print

  movw $done_msg, %si
  call print

  ret

load_error:
  movw $load_debug_fail, %si
  call print

  popa
  movw $load_debug_reg_ax, %si
  call print
  call print_hex_int16

  movw %bx, %ax
  movw $load_debug_reg_bx, %si
  call print
  call print_hex_int16

  movw %cx, %ax
  movw $load_debug_reg_cx, %si
  call print
  call print_hex_int16

  movw %dx, %ax
  movw $load_debug_reg_dx, %si
  call print
  call print_hex_int16

  movw %es, %ax
  movw $load_debug_reg_dx, %si
  call print
  call print_hex_int16

  # TODO: dump hex values of buffer

load_error_loop:
  jmp load_error_loop


.section .data 
drive_params_msg:
  .ascii "Reading drive parameters..."
  .byte 0

reset_msg: 
  .ascii "Resetting bootdrive..."
  .byte 0

load_msg: 
  .ascii "Loading kernel"
  # .byte 13
  # .byte 10
  .byte 0

load_msg_dot:
  .ascii "."
  .byte 0

load_debug_msg:
  .ascii "  Load: (sector) 0x"
  .byte 0

load_debug_colon:
  .ascii ":"
  .byte 0

load_debug_arrow:
  .ascii " -> (address) "
  .byte 0

load_debug_return:
  .ascii " done, (ret) 0x"
  .byte 0

load_debug_fail:
  .ascii " FAILED"
  .byte 13
  .byte 10
  .byte 0

load_debug_reg_ax:
  .ascii " AX=0x"
  .byte 0

load_debug_reg_bx:
  .ascii " BX=0x"
  .byte 0

load_debug_reg_cx:
  .ascii " CX=0x"
  .byte 0

load_debug_reg_dx:
  .ascii " DX=0x"
  .byte 0


bootdriv_id_msg:
  .ascii "- Boot drive ID: 0x"
  .byte 0

num_drives_msg:
  .ascii "- Number of drives: 0x"
  .byte 0

num_sectors_msg:
  .ascii "- Sectors per Track: 0x"
  .byte 0

num_cylinders_msg:
  .ascii "- Total cylinders: 0x"
  .byte 0

num_heads_msg:
  .ascii "- Total drive heads: 0x"
  .byte 0

newline:
  .byte 13
  .byte 10
  .byte 0


.section .bss
.global bootdriv_id
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
