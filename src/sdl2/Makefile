CFLAGS = -O2
CFLAGS += -I../core
CFLAGS += -I/usr/include/SDL2
CFLAGS += -I/usr/local/include/SDL2
CFLAGS += -I/opt/X11/include
CFLAGS += -D_THREAD_SAFE
CFLAGS += -DNDEBUG
CC = gcc $(CFLAGS)

CPPFLAGS = -std=c++17
CPPFLAGS += -fno-rtti
CPPFLAGS += -fno-exceptions
CPPFLAGS += $(CFLAGS)
CPPFLAGS += -DZ80_DISABLE_NESTCHECK
CPPFLAGS += -DZ80_CALLBACK_WITHOUT_CHECK
CPPFLAGS += -DZ80_CALLBACK_PER_INSTRUCTION
CPPFLAGS += -DZ80_UNSUPPORT_16BIT_PORT
CPPFLAGS += -DZ80_NO_FUNCTIONAL
CPPFLAGS += -DZ80_NO_EXCEPTION
CPP = g++ $(CPPFLAGS)

HEADER_FILES = ../core/*.hpp
HEADER_FILES += ../core/*.h

OBJECTS = vgs0sdl2.o
OBJECTS += vgstone.o
OBJECTS += vgs0math.o
OBJECTS += rom_tndtable.o
OBJECTS += emu76489.o

all:
	make build-`uname`

build-Linux:
	make vgs0 LNFLAG="-L/usr/local/lib -lSDL2"

build-Darwin:
	make vgs0 CPPFLAGS="${CPPFLAGS} `sdl2-config --cflags`" LNFLAG="`sdl2-config --libs`"

clean:
	rm -f $(OBJECTS)
	rm -f vgs0 

vgs0: $(OBJECTS)
	$(CPP) -o vgs0 $(OBJECTS) ${LNFLAG}

vgs0sdl2.o: vgs0sdl2.cpp $(HEADER_FILES) ./Makefile
	$(CPP) -c $<

vgstone.o: ../core/vgstone.c $(HEADER_FILES) ./Makefile
	$(CC) -c $<

vgs0math.o: ../core/vgs0math.c $(HEADER_FILES) ./Makefile
	$(CC) -c $<

rom_tndtable.o: ../core/rom_tndtable.c
	${CC} -c $<

emu76489.o: ../core/emu76489.c ../core/emu76489.h
	${CC} -c $<
