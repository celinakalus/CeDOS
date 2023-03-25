.RECIPEPREFIX = >

LOCAL_BUILD = $(GLOBAL_BUILD)/apps

SRC_DIR := $(shell pwd)

C_SOURCES := $(wildcard common/*.c)
S_SOURCES := $(wildcard common/*.s)

C_OBJECTS := $(patsubst common/%.c,$(LOCAL_BUILD)/common/%.c.o,$(C_SOURCES))
S_OBJECTS := $(patsubst common/%.s,$(LOCAL_BUILD)/common/%.s.o,$(S_SOURCES))
OBJECTS = $(S_OBJECTS) $(C_OBJECTS)

APP_SOURCES := $(wildcard *.c)
APP_OBJECTS := $(patsubst %.c,$(GLOBAL_BUILD)/%.o,$(APP_SOURCES))

DIRS := $(sort $(dir $(OBJECTS) $(APP_OBJECTS)))

$(OBJECTS): | $(DIRS)
$(DIRS):
> $(MKDIR) $(DIRS)

.PHONY: build
build: $(APP_OBJECTS)
$(GLOBAL_BUILD)/%.o: $(LOCAL_BUILD)/%.o $(OBJECTS)
> $(LD) -T link.txt -Map=$(LOG_DIR)/apps_mapfile.txt -N  $^ -o $@ 
# > $(OBJCOPY) -I binary -O elf32-i386 $(GLOBAL_BUILD)/apps.o $(GLOBAL_BUILD)/apps_raw.o
# > $(LD) -T link.txt -r $^ -o $(GLOBAL_BUILD)/apps.o --oformat elf32-i386

$(LOCAL_BUILD)/%.s.o: %.s
> $(AS) -o $@ $<

$(LOCAL_BUILD)/%.o: %.c $(wildcard *.h)
> $(CC) -c -I$(INCLUDE_DIR) -I./common $(CCFLAGS) -o $@ $<

$(LOCAL_BUILD)/%.c.o: %.c $(wildcard *.h)
> $(CC) -c -I$(INCLUDE_DIR) -I./common $(CCFLAGS) -o $@ $<
