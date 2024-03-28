APPS := mmap
OBJDIR := build

mmap_SRCS := mmap.c

CFLAGS  += -Wall
LDFLAGS +=

ifndef OBJDIR
 $(error "OBJDIR is not defined. Aborting")
endif

ifndef APPS
 $(error "APPS is not defined. Aborting")
endif

define EXPAND_APP
$(1)_OBJS := $$(patsubst %.c,$(OBJDIR)/%.o,$$($(1)_SRCS))

$(OBJDIR)/$(1): $$($(1)_OBJS)
	$(CC) $$^ $(LDFLAGS) -o $$@

$(1): $(OBJDIR)/$(1);
endef

.PHONY : all clean

all: $(OBJDIR) $(APPS)
	@echo "Done"

$(OBJDIR):
	mkdir -p $@

$(foreach app,$(APPS),$(eval $(call EXPAND_APP,$(app))))

-include $(OBJDIR)/*.d

$(OBJDIR)/%.o: %.c
	$(CC) -c $(CFLAGS) -MMD -MT$(@) $< -o $@

clean:
	$(RM) -rf $(OBJDIR)
