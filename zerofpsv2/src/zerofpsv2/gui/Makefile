SRC=../../
BINARY=libgui.so
BINDIR=$(SRC)../bin/
LIBS=--shared -L$(BINDIR) -lSDL -lSDL_image -L/usr/X11R6/lib `sdl-config --cflags --libs`
OBJS=$(patsubst %.cpp,%.o,$(wildcard *.cpp))

include $(SRC)config
     

T=$(BINDIR)$(BINARY)

all: $T

$T: $(OBJS)
	$(CC) $(C_ARGS) -o $@ $(OBJS) $(LIBS)

clean:
	rm -f *.o
	rm -f $(BINDIR)$(BINARY)
	
