.RECIPEPREFIX = >

# directory definitions
export CURRENT_DIR	:= $(shell pwd)
export ROOT_DIR 	:= $(CURRENT_DIR)
export SOURCE_DIR	:= $(CURRENT_DIR)/src
export INCLUDE_DIR	:= $(CURRENT_DIR)/include
export LOG_DIR		:= $(CURRENT_DIR)/log

# path to cross compiler
export CROSS_COMP	:= $(HOME)/opt/cross/bin/i686-elf-

export CC 		:= $(CROSS_COMP)gcc
export LD 		:= $(CROSS_COMP)ld
export AS 		:= $(CROSS_COMP)as
export AR 		:= $(CROSS_COMP)ar
export OBJCOPY 	:= $(CROSS_COMP)objcopy
export OBJDUMP 	:= $(CROSS_COMP)objdump

#export MAKE 		:= make -R

export MKDIR 		:= mkdir -p
export RM			:= rm

GLOBAL_BUILD		:= $(CURRENT_DIR)/build

# common flags
CCFLAGS 			:= $(CCFLAGS) -Wno-write-strings 
CCFLAGS 			:= $(CCFLAGS) -Qn 
CCFLAGS 			:= $(CCFLAGS) -Wall -Wextra -fno-exceptions 
CCFLAGS 			:= $(CCFLAGS) -nostdlib -nostartfiles -ffreestanding 
CCFLAGS 			:= $(CCFLAGS) -mgeneral-regs-only -mno-red-zone
CCFLAGS				:= $(CCFLAGS) --prefix=$(CROSS_COMP)


# debug target
.PHONY: debug
debug: DEBUG 		:= 1


# release target
ifdef DEBUG
CCFLAGS				:= $(CCFLAGS) -D DEBUG -g -O0 
GLOBAL_BUILD 		:= $(GLOBAL_BUILD)/debug
else
CCFLAGS				:= $(CCFLAGS) -O1
GLOBAL_BUILD 		:= $(GLOBAL_BUILD)/release
endif

LOCAL_BUILD 		:= $(GLOBAL_BUILD)/components

export CCFLAGS
export GLOBAL_BUILD

MODULES := boot kernel libcedos shell
OBJECTS := $(patsubst %,$(LOCAL_BUILD)/%.o,$(MODULES)) $(LOCAL_BUILD)/apps_raw.o
DIRS := $(LOCAL_BUILD)

$(MODULES): | $(DIRS)
$(DIRS):
> $(MKDIR) $@

.PHONY: build
build: $(GLOBAL_BUILD)/cedos.img

$(GLOBAL_BUILD)/fat.img: $(MODULES)
# > $(LD) 		$(OBJECTS) -r -T link.txt -Map=$(LOG_DIR)/elf_mapfile.txt --oformat elf32-i386 -o $@
> dd if=/dev/zero of=$@ count=896
> mkfs.fat -n "cedos" -S 512 -s 8 -r 32  $@
> mkdir -p ./mnt
> sudo mount $@ ./mnt
> sudo cp $(LOCAL_BUILD)/kernel.bin ./mnt
> sudo cp $(LOCAL_BUILD)/bin/* ./mnt
> sudo cp ./img-contents/* ./mnt
> du -csh ./mnt/*
> sudo umount ./mnt

$(GLOBAL_BUILD)/cedos.img: $(GLOBAL_BUILD)/fat.img | $(MODULES) 
> dd if=/dev/zero of=$@ count=904
> parted $@ mklabel msdos
> parted $@ mkpart primary FAT32 8s 896s -s
> parted $@ set 1 boot on
> dd if=$< of=$@ seek=8 conv=notrunc
> dd bs=1 if=$(LOCAL_BUILD)/boot.bin of=$@ count=446 conv=notrunc
> dd if=$(LOCAL_BUILD)/boot.bin of=$@ skip=1 seek=1 count=7 conv=notrunc
> python3 binimg.py -w 256 -i $(GLOBAL_BUILD)/cedos.img -o $(GLOBAL_BUILD)/cedos.png
> parted $@ print list all
# > $(LD) 		$(OBJECTS) -T link.txt -Map=$(LOG_DIR)/bin_mapfile.txt --oformat binary --nostdlib  -o $@

.PHONY: logs
logs: $(LOG_DIR)/base.sym $(LOG_DIR)/objdump.txt

.PHONY: boot
boot:
> $(MAKE) GLOBAL_BUILD=$(LOCAL_BUILD) -C src/boot $(LOCAL_BUILD)/boot.bin

.PHONY: kernel
kernel:
> $(MAKE) GLOBAL_BUILD=$(LOCAL_BUILD) -C src/kernel $(LOCAL_BUILD)/kernel.bin

.PHONY: libcedos
libcedos:
> $(MAKE) GLOBAL_BUILD=$(LOCAL_BUILD) -C src/libcedos build

.PHONY: shell
shell:
> $(MAKE) GLOBAL_BUILD=$(LOCAL_BUILD) -C src/shell build

.PHONY: clean
clean:
> $(RM) -r $(CURRENT_DIR)/build/* 2> /dev/null; true

.PHONY: run
run:
> ./run.sh

.PHONY: docs
docs:
> doxygen doxygen.cfg