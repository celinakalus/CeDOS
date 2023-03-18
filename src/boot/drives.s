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

  ret


  # reset bootdrive
.global reset_drive
reset_drive:
  mov $10, %cx
reset_drive_loop:
  push %cx
  movw $reset_msg, %si
  call print

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
.global load_kernel
load_kernel:
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


.section .data 
drive_params_msg:
  .ascii "Reading drive parameters..."
  .byte 0

reset_msg: 
  .ascii "Resetting bootdrive..."
  .byte 0

load_msg: 
  .ascii "Loading kernel..."
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
