.RECIPEPREFIX = >

GIT_VERSION := "$(shell git describe --abbrev=4 --dirty --always --tags)"
CCFLAGS += -DVERSION=\"$(GIT_VERSION)\"

LOCAL_BUILD = $(GLOBAL_BUILD)/shell/build

SRC_DIR := $(shell pwd)

CCFLAGS += -I../libcedos/include
LDFLAGS += -L$(GLOBAL_BUILD)/lib
LDFLAGS += -lcedos

LDFLAGS += -T link.txt
LDFLAGS += -Map=$(LOG_DIR)/$(notdir $@)_mapfile.txt
LDFLAGS += -N

APP_SOURCES := $(wildcard *.c)
APP_OBJECTS := $(patsubst %.c,$(GLOBAL_BUILD)/bin/%,$(APP_SOURCES)) $(patsubst %.c,$(LOCAL_BUILD)/%.c.o,$(APP_SOURCES))

DIRS := $(sort $(dir $(APP_OBJECTS)))

$(APP_OBJECTS): | $(DIRS)
$(DIRS):
> $(MKDIR) $(DIRS)

.PHONY: build
build: $(APP_OBJECTS)
$(GLOBAL_BUILD)/bin/%: $(LOCAL_BUILD)/%.c.o | $(LIBCEDOS)
> $(LD) $^ $(LDFLAGS) -o $@ 

$(LOCAL_BUILD)/%.c.o: %.s
> $(AS) -o $@ $<

$(LOCAL_BUILD)/%.c.o: %.c $(wildcard *.h)
> $(CC) -c -fPIC -I$(INCLUDE_DIR) -I./common $(CCFLAGS) -o $@ $<
