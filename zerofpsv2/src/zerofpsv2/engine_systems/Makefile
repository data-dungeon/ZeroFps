SRC=../../
BINARY=libengine_systems.so
BINDIR=$(SRC)../bin/
LIBS=--shared -L$(BINDIR) -lSDL_net -lopenal -logg -lvorbis -lvorbisfile -lGL -lGLU -lphysics_engine -L/usr/X11R6/lib `sdl-config --cflags --libs`

OBJS=$(patsubst %.cpp,%.o,$(wildcard audio/*.cpp common/*.cpp mad/*.cpp physicsengine/*.cpp propertys/*.cpp propertys/psproperties/*.cpp script_interfaces/*.cpp tcs/*.cpp))

include $(SRC)config
     

T=$(BINDIR)$(BINARY)

all: $T

$T: $(OBJS)
	$(CC) $(C_ARGS) -o $@ $(OBJS) $(LIBS)

clean:
	rm -f $(OBJS)
	rm -f $(BINDIR)$(BINARY)
	
