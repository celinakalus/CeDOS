.RECIPEPREFIX = >

LOCAL_BUILD = $(GLOBAL_BUILD)/stage1

OBJECTS := $(GLOBAL_BUILD)/stage1.o
DIRS := $(dir $(OBJECTS))

$(OBJECTS): | $(DIRS)
$(DIRS):
> $(MKDIR) $(DIRS)

.PHONY: build
build: $(OBJECTS)
$(GLOBAL_BUILD)/stage1.o: stage1.s
> $(AS) -o $@ $<
