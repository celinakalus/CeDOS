.RECIPEPREFIX = >

# directory definitions
export CURRENT_DIR	:= $(shell pwd)
export ROOT_DIR 	:= $(CURRENT_DIR)
export SOURCE_DIR	:= $(CURRENT_DIR)/src
export INCLUDE_DIR	:= $(CURRENT_DIR)/include
export BUILD_LOGS		:= $(CURRENT_DIR)/log

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
CCFLAGS 			:= $(CCFLAGS) -pedantic -Wold-style-definition 
CCFLAGS 			:= $(CCFLAGS) -Wall -Wextra -fno-exceptions 
CCFLAGS 			:= $(CCFLAGS) -Werror-implicit-function-declaration
CCFLAGS 			:= $(CCFLAGS) -nostdlib -nostartfiles -ffreestanding 
CCFLAGS 			:= $(CCFLAGS) -mgeneral-regs-only -mno-red-zone
CCFLAGS				:= $(CCFLAGS) --prefix=$(CROSS_COMP)

GIT_VERSION			:= "$(shell git describe --abbrev=4 --dirty --always --tags)"
CCFLAGS				:= $(CCFLAGS) -DVERSION=\"$(GIT_VERSION)\"


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

ifndef VERBOSE
.SILENT:
> echo "Silent mode active."
endif

BUILD_OBJECTS 		:= $(GLOBAL_BUILD)/obj
BUILD_ARTIFACTS		:= $(GLOBAL_BUILD)/artifacts
BUILD_LOGS			:= $(GLOBAL_BUILD)/logs
BUILD_MOUNT			:= $(GLOBAL_BUILD)/mnt

export CCFLAGS
export GLOBAL_BUILD

SRC_ALL				:= $(shell find common/ boot/ kernel/ libcedos/ shell/ -name '*.c')
ASM_ALL				:= $(shell find common/ boot/ kernel/ libcedos/ shell/ -name '*.s')

OBJECTS				:= $(patsubst %,$(BUILD_OBJECTS)/%.o,$(SRC_ALL) $(ASM_ALL))


DIRS := $(dir $(OBJECTS)) $(BUILD_ARTIFACTS) $(BUILD_LOGS)
DIRS := $(sort $(DIRS))

$(DIRS):
> $(MKDIR) $@
ifndef VERBOSE
> echo "MKDIR   $@"
endif

.PHONY: build
build: $(BUILD_ARTIFACTS)/cedos.img
$(OBJECTS): $(DIRS)

$(BUILD_OBJECTS)/%.s.o: %.s
> $(AS) -o $@ $<
ifndef VERBOSE
> echo "AS      $@"
endif

# common
OBJ_COMMON := $(filter $(BUILD_OBJECTS)/common/%,$(OBJECTS))
$(BUILD_OBJECTS)/common/%.c.o: common/%.c
> $(CC) -c -Icommon $(CCFLAGS) -o $@ $<


# boot
OBJ_BOOT := $(filter $(BUILD_OBJECTS)/boot/%,$(OBJECTS))
OUT_BOOT := $(BUILD_ARTIFACTS)/boot.elf $(BUILD_ARTIFACTS)/boot.bin

$(BUILD_OBJECTS)/boot/%.c.o: boot/%.c
> $(CC) -c -Iboot -Icommon $(CCFLAGS) -o $@ $<
ifndef VERBOSE
> echo "CC      $@"
endif

$(BUILD_ARTIFACTS)/boot.elf: $(OBJ_BOOT) $(OBJ_COMMON)
> $(LD) $^ -T boot/link.txt -Map=$(BUILD_LOGS)/boot_mapfile.txt --oformat elf32-i386 -o $@
ifndef VERBOSE
> echo "LD      $@"
endif

$(BUILD_ARTIFACTS)/boot.bin: $(OBJ_BOOT) $(OBJ_COMMON)
> $(LD) $^ -T boot/link.txt -Map=$(BUILD_LOGS)/boot_mapfile.txt --oformat binary -o $@
ifndef VERBOSE
> echo "LD      $@"
endif

.PHONY: boot
boot: $(OUT_BOOT)


# kernel
OBJ_KERNEL := $(filter $(BUILD_OBJECTS)/kernel/%,$(OBJECTS))
OUT_KERNEL := $(BUILD_ARTIFACTS)/kernel.elf $(BUILD_ARTIFACTS)/kernel.bin

$(BUILD_OBJECTS)/kernel/%.c.o: kernel/%.c
> $(CC) -c -Ikernel -Icommon $(CCFLAGS) -o $@ $<
ifndef VERBOSE
> echo "CC      $@"
endif

$(BUILD_ARTIFACTS)/kernel.elf: $(OBJ_KERNEL) $(OBJ_COMMON)
> $(LD) $^ -T kernel/link.txt -Map=$(BUILD_LOGS)/kernel_mapfile.txt --oformat elf32-i386 -o $@
ifndef VERBOSE
> echo "LD      $@"
endif

$(BUILD_ARTIFACTS)/kernel.bin: $(OBJ_KERNEL) $(OBJ_COMMON)
> $(LD) $^ -T kernel/link.txt -Map=$(BUILD_LOGS)/kernel_mapfile.txt --oformat binary -o $@
ifndef VERBOSE
> echo "LD      $@"
endif

.PHONY: kernel
kernel: $(OUT_KERNEL)


# libcedos
OBJ_LIBCEDOS := $(filter $(BUILD_OBJECTS)/libcedos/%,$(OBJECTS))
OUT_LIBCEDOS := $(BUILD_ARTIFACTS)/libcedos.a

$(BUILD_OBJECTS)/libcedos/%.c.o: libcedos/%.c
> $(CC) -c -Ilibcedos -Icommon $(CCFLAGS) -o $@ $<
ifndef VERBOSE
> echo "CC      $@"
endif

$(BUILD_ARTIFACTS)/libcedos.a: $(OBJ_LIBCEDOS) $(OBJ_COMMON)
> $(AR) rcs $@ $^
ifndef VERBOSE
> echo "AR      $@"
endif

.PHONY: libcedos
libcedos: $(OUT_LIBCEDOS)


# shell
OBJ_SHELL := $(filter $(BUILD_OBJECTS)/shell/%,$(OBJECTS))
OUT_SHELL := $(patsubst $(BUILD_OBJECTS)/shell/%.c.o,$(BUILD_ARTIFACTS)/%,$(OBJ_SHELL))

SHELL_CCFLAGS += -I../libcedos/include
SHELL_LDFLAGS += -L$(BUILD_ARTIFACTS)
SHELL_LDFLAGS += -lcedos

SHELL_LDFLAGS += -T shell/link.txt
SHELL_LDFLAGS += -Map=$(BUILD_LOGS)/$(notdir $@)_mapfile.txt
SHELL_LDFLAGS += -N

$(BUILD_OBJECTS)/shell/%.c.o: shell/%.c
> $(CC) -c -Ishell -Ilibcedos -Icommon $(CCFLAGS) $(SHELL_CCFLAGS) -o $@ $<
ifndef VERBOSE
> echo "CC      $@"
endif

$(BUILD_ARTIFACTS)/%: $(BUILD_OBJECTS)/shell/%.c.o | $(BUILD_ARTIFACTS)/libcedos.a
> $(LD) $^ $(LDFLAGS) $(SHELL_LDFLAGS) -o $@ 
ifndef VERBOSE
> echo "LD      $@"
endif

.PHONY: shell
shell: $(OUT_SHELL)


# disk image
$(BUILD_ARTIFACTS)/fat.img: $(filter %.bin,$(OUT_KERNEL)) $(OUT_SHELL) | $(BUILD_ARTIFACTS)/boot.bin
# > $(LD) 		$(OBJECTS) -r -T link.txt -Map=$(BUILD_LOGS)/elf_mapfile.txt --oformat elf32-i386 -o $@
> dd if=/dev/zero of=$@ count=896
> mkfs.fat -n "cedos" -S 512 -s 8 -r 64  $@
> mkdir -p $(BUILD_MOUNT)
> sudo mount $@ $(BUILD_MOUNT)
> sudo cp $^ $(BUILD_MOUNT)
> sudo cp ./img-contents/* $(BUILD_MOUNT) || echo "No img-contents folder; Skipping."
> du -csh $(BUILD_MOUNT)/*
> sudo umount $(BUILD_MOUNT)
> rm -r $(BUILD_MOUNT)
ifndef VERBOSE
> echo "MKFS    $@"
endif

$(BUILD_ARTIFACTS)/cedos.img: $(BUILD_ARTIFACTS)/fat.img | $(MODULES) 
> dd if=/dev/zero of=$@ count=904
> parted $@ mklabel msdos
> parted $@ mkpart primary FAT32 8s 896s -s
> parted $@ set 1 boot on
> dd if=$< of=$@ seek=8 conv=notrunc
> dd bs=1 if=$(BUILD_ARTIFACTS)/boot.bin of=$@ count=446 conv=notrunc
> dd if=$(BUILD_ARTIFACTS)/boot.bin of=$@ skip=1 seek=1 count=7 conv=notrunc
> python3 binimg.py -w 256 -i $(BUILD_ARTIFACTS)/cedos.img -o $(BUILD_ARTIFACTS)/cedos.png
> parted $@ print list all
# > $(LD) 		$(OBJECTS) -T link.txt -Map=$(BUILD_LOGS)/bin_mapfile.txt --oformat binary --nostdlib  -o $@
ifndef VERBOSE
> echo "MKPART  $@"
endif

.PHONY: logs
logs: $(BUILD_LOGS)/base.sym $(BUILD_LOGS)/objdump.txt

.PHONY: clean
clean:
> $(RM) -r $(CURRENT_DIR)/build/* 2> /dev/null; true

.PHONY: run
run:
> ./run.sh $(BUILD_ARTIFACTS)/cedos.img

.PHONY: docs
docs:
> doxygen doxygen.cfg