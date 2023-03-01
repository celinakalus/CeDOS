.RECIPEPREFIX = >

export CURRENT_DIR		= $(shell pwd)
export INCLUDE_DIR		= $(CURRENT_DIR)/include
export LOG_DIR		= $(CURRENT_DIR)/log

export GCC_PREFIX		= $(HOME)/opt/cross/bin/i686-elf-

GLOBAL_BUILD		= $(CURRENT_DIR)/build
ifdef DEBUG
GCC_OPTIONS		= -D DEBUG -g -O0 -Wno-write-strings -Qn -Wall -Wextra -fno-exceptions -nostdlib -nostartfiles -ffreestanding -mgeneral-regs-only -mno-red-zone
LOCAL_BUILD = $(GLOBAL_BUILD)/debug
else
GCC_OPTIONS		= -O1 -Wno-write-strings -Qn -Wall -Wextra -fno-exceptions -nostdlib -nostartfiles -ffreestanding -mgeneral-regs-only -mno-red-zone
LOCAL_BUILD = $(GLOBAL_BUILD)/release
endif

export GCC_OPTIONS

.PHONY: folder
folder:
> @mkdir -p $(LOCAL_BUILD)

.PHONY: build
build: boot kernel apps
> @mkdir $(LOCAL_BUILD) 2> /dev/null; true
> $(GCC_PREFIX)ld $(LOCAL_BUILD)/*.o -T link.txt -Map=$(LOG_DIR)/mapfile.txt -o $(GLOBAL_BUILD)/base.o
> $(GCC_PREFIX)objcopy --only-keep-debug $(GLOBAL_BUILD)/base.o $(LOG_DIR)/base.sym
> $(GCC_PREFIX)objcopy -O binary $(GLOBAL_BUILD)/base.o $(GLOBAL_BUILD)/base.img
> $(GCC_PREFIX)objdump -D $(GLOBAL_BUILD)/base.o > $(LOG_DIR)/objdump.txt

.PHONY: boot
boot:
> $(MAKE) GLOBAL_BUILD=$(LOCAL_BUILD) -C boot build

.PHONY: kernel
kernel:
> $(MAKE) GLOBAL_BUILD=$(LOCAL_BUILD) -C kernel build

.PHONY: apps
apps:
> $(MAKE) GLOBAL_BUILD=$(LOCAL_BUILD) -C apps build

.PHONY: clean
clean:
> @rm -r $(CURRENT_DIR)/build/* 2> /dev/null; true

.PHONY: run
run:
> ./run.sh

.PHONY: debug
debug:
> $(MAKE) DEBUG=1 build

.PHONY: docs
docs:
> doxygen doxygen.cfg