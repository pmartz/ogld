$($(X_MODULE)_OUTPUT)/%.obj: $(X_MODULE)/%.cpp
	$(COMPILE.cpp) -O2 -s -Wall -IOGLD/include -o '$@' '$<'

$($(X_MODULE)_OUTPUT)/$(BINARY).lib: $($(X_MODULE)_OBJS)
	$(AR) r '$@' $^
	ranlib '$@'

$($(X_MODULE)_OUTPUT)/$(BINARY)$(X_EXEEXT): $($(X_MODULE)_OBJS)
	$(LINK.cpp) -L$(X_OUTARCH)/OGLD $^ -llibOGLD -ltiff -lglut32 -lglu32 -lopengl32 -lgdi32 -o '$@'
