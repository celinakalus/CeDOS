qemu-system-i386 -drive index=0,if=floppy,format=raw,file=${1} -m 64 -monitor stdio -no-reboot -d int,cpu_reset,exec,in_asm -vga std -vnc :0 2> log/run_err.log
