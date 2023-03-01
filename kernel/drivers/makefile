.RECIPEPREFIX = >

S_OBJECTS = $(patsubst %.s,$(LOCAL_BUILD)/%.s.o,$(wildcard *.s))
C_OBJECTS = $(patsubst %.c,$(LOCAL_BUILD)/%.c.o,$(wildcard *.c))
OBJECTS = $(S_OBJECTS) $(C_OBJECTS)

LOCAL_BUILD = $(GLOBAL_BUILD)/drivers

SUBDIRS = $(wildcard */.)

.PHONY: build
build: folder $(SUBDIRS) $(OBJECTS)
> $(LD) $(wildcard $(LOCAL_BUILD)/*.o) -r -o $(GLOBAL_BUILD)/drivers.o --oformat elf32-i386

.PHONY: folder
folder:
> $(MKDIR) $(LOCAL_BUILD)

.PHONY: $(SUBDIRS)
$(SUBDIRS):
> $(MAKE) -C $@ GLOBAL_BUILD=$(LOCAL_BUILD) build

$(LOCAL_BUILD)/%.s.o: %.s
> $(AS) -o $@ $<

$(LOCAL_BUILD)/%.c.o: %.c $(wildcard *.h)
> $(CC) -c -I$(INCLUDE_DIR) $(CCFLAGS) -o $@ $<
