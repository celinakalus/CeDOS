# CeDOS
Simple x86 operating system written in assembly and C.

## Software Requirements
* GNU make
* GCC Cross Compiler + matching binutils, tailored for freestanding i686-elf-targets
* Doxygen: If you want to build the documentation

## Make commands
### Debug build
```make debug```

### Release build
```make build```

### Clear output directories
```make clear```

### Build documentation:
```make docs```

## Tested build environment versions
### 2023-03-01
* GCC 12.2.0
* binutils 2.40
* QEMU 4.2.1