SRC=../../../
BINARY=madmake.bin
BINDIR=$(SRC)../bin/
#LIBS=-L$(BINDIR) -L/usr/X11R6/lib -lbasic -lSDL
LIBS=-L$(BINDIR) -L/usr/X11R6/lib -lengine_systems -lbasic -lengine -lrender -lscript -lgui -lmcommon

#SOURCE=$(wildcard *.cpp)
SOURCE=mad.cpp madmake.cpp main.cpp qpack.cpp xxx.cpp obj.cpp
# ../../../zerofpsv2/engine_systems/mad/core.cpp ../../../zerofpsv2/engine_systems/mad/core_boneanim.cpp ../../../zerofpsv2/engine_systems/mad/core_bone.cpp ../../../zerofpsv2/engine_systems/mad/core_meshanim.cpp ../../../zerofpsv2/engine_systems/mad/core_mesh.cpp
OBJS=$(SOURCE:.cpp=.o)
DEP=$(SOURCE:.cpp=.d)

include $(SRC)config      


T=$(BINDIR)$(BINARY)
all: $T

$T: $(OBJS)
	$(CC) $(C_ARGS) -o $@ $(OBJS) $(LIBS)

%.d:%.cpp
	$(CC) -MM $*.cpp |sed 's/.o:/.o $(subst /,\/,$(notdir $*)).d:/'  >$@
	
clean:
	rm -f $(OBJS)
	rm -f $(BINDIR)$(BINARY)
	rm -f $(DEP)

include $(DEP)
