SRC=../
BINARY=glinfo
BINDIR=$(SRC)../bin/
LIBS=-L$(BINDIR) -L/usr/X11R6/lib -lGL -lSDL

OBJS=$(patsubst %.cpp,%.o,$(wildcard *.cpp))

include $(SRC)config      

T=$(BINDIR)$(BINARY)

all: $T

$T: $(OBJS)
	$(CC) $(C_ARGS) -o $@ $(OBJS) $(LIBS)

clean:
	rm -f $(OBJS)
	rm -f $(BINDIR)$(BINARY)
	
