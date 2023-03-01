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

.PHONY: folder
folder:
> $(MKDIR) $(LOCAL_BUILD)

.PHONY: build
build: $(GLOBAL_BUILD)/base.img

$(GLOBAL_BUILD)/base.o: boot kernel apps
> $(LD) 		$(LOCAL_BUILD)/*.o -T link.txt -Map=$(LOG_DIR)/mapfile.txt -o $(GLOBAL_BUILD)/base.o

$(GLOBAL_BUILD)/base.img: $(GLOBAL_BUILD)/base.o
> $(OBJCOPY) 	-O binary $(GLOBAL_BUILD)/base.o $(GLOBAL_BUILD)/base.img

.PHONY: logs
logs: $(LOG_DIR)/base.sym $(LOG_DIR)/objdump.txt

$(LOG_DIR)/base.sym: $(GLOBAL_BUILD)/base.o
> $(OBJCOPY) 	--only-keep-debug $(GLOBAL_BUILD)/base.o $(LOG_DIR)/base.sym

$(LOG_DIR)/objdump.txt: $(GLOBAL_BUILD)/base.o
> $(OBJDUMP) 	-D $(GLOBAL_BUILD)/base.o > $(LOG_DIR)/objdump.txt

.PHONY: boot
boot:
> echo $(AS)
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

.PHONY: docs
docs:
> doxygen doxygen.cfg