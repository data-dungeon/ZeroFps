SRC=../../
BINARY=libengine_systems.so
BINDIR=$(SRC)../bin/
LIBS=--shared -L$(BINDIR) -lSDL_net -lopenal -logg -lvorbis -lvorbisfile -L/usr/X11R6/lib `sdl-config --cflags --libs`

SOURCE=$(wildcard audio/*.cpp common/*.cpp mad/*.cpp physicsengine/*.cpp propertys/*.cpp propertys/psproperties/*.cpp script_interfaces/*.cpp tcs/*.cpp)
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

