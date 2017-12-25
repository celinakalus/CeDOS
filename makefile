.RECIPEPREFIX = >

export CURRENT_DIR		= $(shell pwd)
export BUILD_DIR		= $(CURRENT_DIR)/build
export INCLUDE_DIR		= $(CURRENT_DIR)/include
export DEBUG_DIR		= $(CURRENT_DIR)/debug

export GCC_PREFIX		= $(HOME)/opt/cross/i686-elf-/bin/i686-elf-

ifdef DEBUG
GCC_OPTIONS		= -D DEBUG -O0 -Wno-write-strings -Qn -Wall -Wextra -fno-exceptions -nostdlib -nostartfiles -ffreestanding
else
GCC_OPTIONS		= -O1 -Wno-write-strings -Qn -Wall -Wextra -fno-exceptions -nostdlib -nostartfiles -ffreestanding
endif

export GCC_OPTIONS

# OBJ_FILES = $(wildcard obj/asm/*.o) $(wildcard obj/cpp/*.o)

.PHONY: build
build:
> $(MAKE) -C boot build
> $(MAKE) -C second_stage build
> $(MAKE) -C kernel build
> $(GCC_PREFIX)ld -T link.txt -Map=$(DEBUG_DIR)/mapfile.txt -o $(BUILD_DIR)/base.img --oformat binary

.PHONY: clear
clear:
> @rm $(BUILD_DIR)/*.* 2> /dev/null; true
> @$(MAKE) -C boot clear 2> /dev/null; true
> @$(MAKE) -C second_stage clear 2> /dev/null; true
> @$(MAKE) -C kernel clear 2> /dev/null; true

.PHONY: run
run:
> ./run.sh

.PHONY: rebuild
rebuild:
> $(MAKE) $(MAKEFLAGS) clear
> $(MAKE) $(MAKEFLAGS) build

.PHONY: objdump
objdump:
> $(GCC_PREFIX)objdump -D $(FILE) > debug/objdump.txt

.PHONY: debug
debug:
> $(MAKE) DEBUG=1 build
