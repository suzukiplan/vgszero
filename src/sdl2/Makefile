CFLAGS = -O2
CFLAGS += -I../core
CFLAGS += -I/usr/include/SDL2
CFLAGS += -I/usr/local/include/SDL2
CFLAGS += -I/opt/X11/include
CFLAGS += -D_THREAD_SAFE
CC = gcc $(CFLAGS)

CPPFLAGS = -std=c++17
CPPFLAGS += -fno-rtti
CPPFLAGS += -fno-exceptions
CPPFLAGS += $(CFLAGS)
CPPFLAGS += -DZ80_DISABLE_DEBUG
CPPFLAGS += -DZ80_DISABLE_BREAKPOINT
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

all: vgs0

clean:
	rm -f $(OBJECTS)
	rm -f vgs0 

vgs0: $(OBJECTS)
	$(CPP) -o vgs0 $(OBJECTS) -L/usr/local/lib -lSDL2

vgs0sdl2.o: vgs0sdl2.cpp $(HEADER_FILES) ./Makefile
	$(CPP) -c $<

vgstone.o: ../core/vgstone.c $(HEADER_FILES) ./Makefile
	$(CC) -c $<

vgs0math.o: ../core/vgs0math.c $(HEADER_FILES) ./Makefile
	$(CC) -c $<
