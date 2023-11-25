# CeDOS
Simple x86 operating system written in assembly and C.

## Software Requirements
* GNU make
* [https://wiki.osdev.org/GCC_Cross-Compiler](GCC Cross Compiler + matching binutils), tailored for freestanding i686-elf-targets
* Doxygen: If you want to build the documentation
* QEMU: To emulate an x86 system. More specifically, `qemu-system-i386`, available via apt.

## Make commands
Release builds are the default. For debug builds, use `make DEBUG=1 [...]`. You will need to add `DEBUG=1` to all commands, including `run`, otherwise, the release version is run.

### Build
```make build```

### Run (in an emulator)
```make run```

### Clear output directories
```make clean```

### Build documentation:
```make docs```

## Tested build environment versions
### 2023-03-01
* GCC 12.2.0
* binutils 2.40
* QEMU 4.2.1

## Roadmap
[ ] Implement FAT12 support in bootloader
[ ] Add FAT12 kernel driver with syscall interface
[ ] Allow execution of ELF files by file system path
[ ] Implement proper POSIX file interface
[ ] Add a basic shell
[ ] Add functions for VGA graphics