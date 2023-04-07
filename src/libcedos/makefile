.RECIPEPREFIX = >

LOCAL_BUILD = $(GLOBAL_BUILD)/libcedos

SRC_DIR := $(shell pwd)

C_SOURCES := $(wildcard *.c)
S_SOURCES := $(wildcard *.s)

C_OBJECTS := $(patsubst %.c,$(LOCAL_BUILD)/%.c.o,$(C_SOURCES))
S_OBJECTS := $(patsubst %.s,$(LOCAL_BUILD)/%.s.o,$(S_SOURCES))
OBJECTS = $(S_OBJECTS) $(C_OBJECTS)

LIBCEDOS := $(GLOBAL_BUILD)/lib/libcedos.a

DIRS := $(sort $(dir $(OBJECTS) $(LIBCEDOS)))

$(OBJECTS) $(LIBCEDOS): | $(DIRS)
$(DIRS):
> $(MKDIR) $(DIRS)

.PHONY: build
build: $(LIBCEDOS)
$(LIBCEDOS): $(OBJECTS)
> $(AR) rcs $@ $^

$(LOCAL_BUILD)/%.s.o: %.s
> $(AS) -o $@ $<

$(LOCAL_BUILD)/%.c.o: %.c $(wildcard *.h)
> $(CC) -c -fPIC -I$(INCLUDE_DIR) -I./include $(CCFLAGS) -o $@ $<
