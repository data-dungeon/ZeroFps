SRC=../
BINARY=libmcommon.so
BINDIR=$(SRC)../bin/
LIBS=--shared -L$(BINDIR) -L/usr/X11R6/lib -lengine_systems -lbasic -lengine -lrender -lscript -lgui

#OBJS=$(patsubst %.cpp,%.o,$(wildcard *.cpp rulesystem/*.cpp rulesystem/character/*.cpp rulesystem/item/*.cpp rulesystem/spell/*.cpp))
OBJS=$(patsubst %.cpp,%.o,$(wildcard *.cpp))

include $(SRC)config      

T=$(BINDIR)$(BINARY)

all: $T

$T: $(OBJS)
	$(CC) $(C_ARGS) -o $@ $(OBJS) $(LIBS)

clean:
	rm -f $(OBJS)
	rm -f $(BINDIR)$(BINARY)
	
