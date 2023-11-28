.code16
.section .text
.global find_bootable_part
find_bootable_part:
  movw $0x0000, %ax
  movw %ax, %ds

  movw $0x0000, %cx

find_part_loop:
  movw $mbr_partition_msg, %si
  call print
  movw %cx, %ax
  call print_hex_int16
  movw $newline, %si
  call print

  movw $PART_TABLE_START, %si
  
  xor %ax, %ax
  movb (%si), %al

  cmp $0x80, %al
  je find_part_loop_end

  # TODO: loop over partitions
  jmp find_part_loop_fail

find_part_loop_end:
  movw $mbr_bootable_msg, %si
  call print
  movw %cx, %ax
  call print_hex_int16
  movw $newline, %si
  call print

  # load partition size from MBR
  movw $PART_TABLE_START, %si
  add $0x0C, %si

  movw (%si), %cx

  movw $mbr_size_msg, %si
  call print
  movw %cx, %ax
  call print_hex_int16
  movw $newline, %si
  call print

  # load partition start from MBR
  movw $PART_TABLE_START, %si
  add $0x08, %si

  movw (%si), %ax

  movw $mbr_start_msg, %si
  call print
  call print_hex_int16
  movw $newline, %si
  call print

  add %ax, %cx

  ret

find_part_loop_fail:
  jmp error

.section .data
mbr_partition_msg:
  .ascii "  Checking partition 0x"
  .byte

mbr_bootable_msg:
  .ascii "  bootable partition found: 0x"
  .byte 0

mbr_start_msg:
  .ascii "  Start sector: 0x"
  .byte 0

mbr_size_msg:
  .ascii "  Number of sectors: 0x"
  .byte 0

newline:
  .byte 13
  .byte 10
  .byte 0
