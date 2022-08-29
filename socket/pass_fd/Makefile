APPS := sendfd recvfd

sendfd_SRCS := sendfd.c
recvfd_SRCS := recvfd.c

define EXPAND_VAR
$(1)_OBJS :=  $$(patsubst %.c,%.o,$$($(1)_SRCS))
$(1): $$($(1)_OBJS)
	$(CC) -o $$@ $$($(1)_OBJS) $$(LDFLAGS)
endef

.PHONY : all clean

CFLAGS  += -Wall
LDFLAGS +=

all: .prepare $(APPS)

.prepare:
	echo -n "Hi" > ./HelloWorld
	touch .prepare

$(foreach app,$(APPS),$(eval $(call EXPAND_VAR,$(app))))

clean:
	$(RM) -rf *.o .prepare HelloWorld un_socket $(APPS)
