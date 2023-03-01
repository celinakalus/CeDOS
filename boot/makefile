.RECIPEPREFIX = >

S_OBJECTS = $(patsubst %.s,$(LOCAL_BUILD)/%.s.o,$(wildcard *.s))
C_OBJECTS = $(patsubst %.c,$(LOCAL_BUILD)/%.c.o,$(wildcard *.c))
OBJECTS = $(S_OBJECTS) $(C_OBJECTS)

LOCAL_BUILD = $(GLOBAL_BUILD)

SUBDIRS = $(wildcard */.)

.PHONY: folder
folder:
> @mkdir -p $(LOCAL_BUILD)

.PHONY: build
build: folder $(SUBDIRS)

.PHONY: $(SUBDIRS)
$(SUBDIRS):
> $(MAKE) -C $@ GLOBAL_BUILD=$(LOCAL_BUILD) build