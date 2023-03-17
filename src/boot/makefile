.RECIPEPREFIX = >

LOCAL_BUILD := $(GLOBAL_BUILD)/boot

SRC_DIR := $(shell pwd)

C_SOURCES := $(wildcard *.c)
S_SOURCES := $(wildcard *.s)

C_OBJECTS := $(patsubst %.c,$(LOCAL_BUILD)/%.c.o,$(C_SOURCES))
S_OBJECTS := $(patsubst %.s,$(LOCAL_BUILD)/%.s.o,$(S_SOURCES))
OBJECTS = $(S_OBJECTS) $(C_OBJECTS)

DIRS := $(sort $(dir $(OBJECTS)))

$(OBJECTS): | $(DIRS)
$(DIRS):
> $(MKDIR) $(DIRS)

.PHONY: build
build: $(GLOBAL_BUILD)/boot.o
$(GLOBAL_BUILD)/boot.o: $(OBJECTS)
> $(LD) $^ -r -T link.txt -Map=$(LOG_DIR)/boot_mapfile.txt -o $@ --oformat elf32-i386

$(LOCAL_BUILD)/%.s.o: %.s
> $(AS) -o $@ $<

$(LOCAL_BUILD)/%.c.o: %.c $(wildcard *.h)
> $(CC) -c $(CCFLAGS) -o $@ $<