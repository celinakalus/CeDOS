qemu-system-i386 -drive index=0,if=floppy,format=raw,file=./build/release/cedos.img -m 64 -monitor stdio -no-reboot -d int,cpu_reset,exec,in_asm -vnc :0 2> log/run_err.log
