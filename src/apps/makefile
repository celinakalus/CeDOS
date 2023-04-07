.RECIPEPREFIX = >

LOCAL_BUILD = $(GLOBAL_BUILD)/apps

SRC_DIR := $(shell pwd)

C_SOURCES := $(wildcard common/*.c)
S_SOURCES := $(wildcard common/*.s)

C_OBJECTS := $(patsubst common/%.c,$(LOCAL_BUILD)/common/%.c.o,$(C_SOURCES))
S_OBJECTS := $(patsubst common/%.s,$(LOCAL_BUILD)/common/%.s.o,$(S_SOURCES))
LIB_OBJECTS = $(S_OBJECTS) $(C_OBJECTS)

LIBCEDOS := $(LOCAL_BUILD)/lib/libcedos.a

LDFLAGS += -L$(LOCAL_BUILD)/lib
LDFLAGS += -lcedos

LDFLAGS += -T link.txt
LDFLAGS += -Map=$(LOG_DIR)/$(notdir $@)_mapfile.txt
LDFLAGS += -N

APP_SOURCES := $(wildcard *.c)
APP_OBJECTS := $(patsubst %.c,$(GLOBAL_BUILD)/%.o,$(APP_SOURCES))

DIRS := $(sort $(dir $(LIB_OBJECTS) $(APP_OBJECTS) $(LIBCEDOS)))

$(APP_OBJECTS) $(LIB_OBJECTS) $(LIBCEDOS): | $(DIRS)
$(DIRS):
> $(MKDIR) $(DIRS)

.PHONY: build
build: $(APP_OBJECTS) $(LIBCEDOS)
$(GLOBAL_BUILD)/%.o: $(LOCAL_BUILD)/%.c.o | $(LIBCEDOS)
> $(LD) $^ $(LDFLAGS) -o $@ 
# > $(OBJCOPY) -I binary -O elf32-i386 $(GLOBAL_BUILD)/apps.o $(GLOBAL_BUILD)/apps_raw.o
# > $(LD) -T link.txt -r $^ -o $(GLOBAL_BUILD)/apps.o --oformat elf32-i386

$(LIBCEDOS): $(LIB_OBJECTS)
> $(AR) rcs $@ $^

$(LOCAL_BUILD)/%.s.o: %.s
> $(AS) -o $@ $<

$(LOCAL_BUILD)/%.c.o: %.c $(wildcard *.h)
> $(CC) -c -fPIC -I$(INCLUDE_DIR) -I./common $(CCFLAGS) -o $@ $<
