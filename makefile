.RECIPEPREFIX = >

# directory definitions
export CURRENT_DIR	:= $(shell pwd)
export ROOT_DIR 	:= $(CURRENT_DIR)
export INCLUDE_DIR	:= $(CURRENT_DIR)/include
export LOG_DIR		:= $(CURRENT_DIR)/log

# path to cross compiler
export CROSS_COMP	:= $(HOME)/opt/cross/bin/i686-elf-

export CC 		:= $(CROSS_COMP)gcc
export LD 		:= $(CROSS_COMP)ld
export AS 		:= $(CROSS_COMP)as
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

OBJECTS := $(LOCAL_BUILD)/stage1.o $(LOCAL_BUILD)/stage2.o $(LOCAL_BUILD)/kernel.o $(LOCAL_BUILD)/apps.o
DIRS := $(dir $(OBJECTS))

$(OBJECTS): | $(DIRS)
$(DIRS):
> $(MKDIR) $(DIRS)

.PHONY: build
build: $(GLOBAL_BUILD)/base.img $(GLOBAL_BUILD)/base.o

$(GLOBAL_BUILD)/base.o: $(OBJECTS)
> $(LD) 		$^ -r -T link.txt -Map=$(LOG_DIR)/mapfile.txt --oformat elf32-i386 -o $@

$(GLOBAL_BUILD)/base.img: $(OBJECTS)
> $(LD) 		$^ -T link.txt -Map=$(LOG_DIR)/mapfile.txt --oformat binary --nostdlib  -o $@

.PHONY: logs
logs: $(LOG_DIR)/base.sym $(LOG_DIR)/objdump.txt

$(LOG_DIR)/base.sym: $(GLOBAL_BUILD)/base.o
> $(OBJCOPY) 	--only-keep-debug $< $@

$(LOG_DIR)/objdump.txt: $(GLOBAL_BUILD)/base.o
> $(OBJDUMP) 	-D $< > $@

.PHONY: stage1
stage1:
$(LOCAL_BUILD)/stage1.o:
> $(MAKE) GLOBAL_BUILD=$(LOCAL_BUILD) -C stage1 $@
.PHONY: stage2

stage2:
$(LOCAL_BUILD)/stage2.o:
> $(MAKE) GLOBAL_BUILD=$(LOCAL_BUILD) -C stage2 $@

.PHONY: kernel
kernel:
$(LOCAL_BUILD)/kernel.o:
> $(MAKE) GLOBAL_BUILD=$(LOCAL_BUILD) -C kernel $@

.PHONY: apps
apps:
$(LOCAL_BUILD)/apps.o:
> $(MAKE) GLOBAL_BUILD=$(LOCAL_BUILD) -C apps $@

.PHONY: clean
clean:
> $(RM) -r $(CURRENT_DIR)/build/* 2> /dev/null; true

.PHONY: run
run:
> ./run.sh

.PHONY: docs
docs:
> doxygen doxygen.cfg