SRC=../
BINARY=libcommon.so
BINDIR=$(SRC)../bin/
LIBS=--shared -L$(BINDIR) -L../../bin/ -lbasic -lengine -lrender -lscript -lgui -lphysics_engine

OBJS=$(patsubst %.cpp,%.o,$(wildcard *.cpp))

include $(SRC)config      

T=$(BINDIR)$(BINARY)

all: $T

$T: $(OBJS)
	$(CC) $(C_ARGS) -o $@ $(OBJS) $(LIBS)

clean:
	rm -f *.o
	rm -f $(BINDIR)$(BINARY)
	
