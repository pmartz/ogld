include $(X_ARCH).mak
$(X_MODULE)_OBJS = $(addsuffix $(X_OBJEXT),$(addprefix $($(X_MODULE)_OUTPUT)/,$(basename $(SRCS)))) $(DEPS)
$(X_MODULE)_BINARY = $(addprefix $($(X_MODULE)_OUTPUT)/,$(BINARY))$(BINARY_EXT)
include $(X_ARCH)-rules.mak

all: $($(X_MODULE)_BINARY)
$(X_MODULE): $($(X_MODULE)_BINARY)

