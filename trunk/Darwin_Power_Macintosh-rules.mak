$($(X_MODULE)_OUTPUT)/%.o: $(X_MODULE)/%.cpp
	$(COMPILE.cpp) -g -Wall -IOGLD/include -I/System/Library/Frameworks/GLUT.framework/Headers -o '$@' '$<'

$($(X_MODULE)_OUTPUT)/$(BINARY).a: $($(X_MODULE)_OBJS)
	$(AR) r '$@' $^
	ranlib '$@'

$($(X_MODULE)_OUTPUT)/$(BINARY)$(X_EXEEXT): $($(X_MODULE)_OBJS)
	$(LINK.cpp) -L$(X_OUTARCH)/OGLD -framework GLUT -framework OpenGL -framework AGL -framework Carbon $^ -lOGLD -ltiff -o '$@'
