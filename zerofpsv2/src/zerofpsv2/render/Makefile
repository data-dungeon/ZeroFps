SRC=../../
BINARY=librender.so
BINDIR=$(SRC)../bin/
LIBS=--shared -L$(BINDIR) -lSDL -lGL -lGLU  -L/usr/X11R6/lib `sdl-config --cflags --libs`

SOURCE=$(wildcard *.cpp)
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