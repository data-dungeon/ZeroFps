SRC=../../
BINARY=libscript.so
BINDIR=$(SRC)../bin/
LIBS=--shared -L$(BINDIR) -llua -llualib

OBJS=$(patsubst %.cpp,%.o,$(wildcard *.cpp))

include $(SRC)config
     

T=$(BINDIR)$(BINARY)

all: $T

$T: $(OBJS)
	$(CC) $(C_ARGS) -o $@ $(OBJS) $(LIBS)

clean:
	rm -f $(OBJS)
	rm -f $(BINDIR)$(BINARY)
	
