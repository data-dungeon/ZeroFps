SRC=../
BINARY=mistclient
BINDIR=$(SRC)../bin/
LIBS=-L$(BINDIR) -L/usr/X11R6/lib -lengine_systems -lphysics_engine -lbasic -lengine -lrender -lscript -lgui -lglut

OBJS=$(patsubst %.cpp,%.o,$(wildcard *.cpp))

include $(SRC)config      

T=$(BINDIR)$(BINARY)

all: $T

$T: $(OBJS)
	$(CC) $(C_ARGS) -o $@ $(OBJS) $(LIBS)

clean:
	rm -f *.o
	rm -f $(BINDIR)$(BINARY)
	
