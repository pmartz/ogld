$($(X_MODULE)_OUTPUT)/%.o: $(X_MODULE)/%.cpp
	$(COMPILE.cpp) -g -Wall -O2 -ansi -pedantic -DGLX -IOGLD/include -o '$@' '$<'

$($(X_MODULE)_OUTPUT)/$(BINARY).a: $($(X_MODULE)_OBJS)
	$(AR) r '$@' $^
	ranlib '$@'

$($(X_MODULE)_OUTPUT)/$(BINARY)$(X_EXEEXT): $($(X_MODULE)_OBJS)
	$(LINK.cpp) -L$(X_OUTARCH)/OGLD $^ -lOGLD -ltiff -lglut -lGLU -lGL -ldl -lX11 -o '$@'
