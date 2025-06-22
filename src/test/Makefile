CFLAGS = -O2
CFLAGS += -I../core
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

OBJECTS = vgs0test.o
OBJECTS += vgstone.o
OBJECTS += vgs0math.o
OBJECTS += rom_tndtable.o
OBJECTS += emu76489.o

all: vgs0test

clean:
	rm -f $(OBJECTS)
	rm -f vgs0test

vgs0test: $(OBJECTS)
	$(CPP) -o vgs0test $(OBJECTS)

vgs0test.o: vgs0test.cpp $(HEADER_FILES) ./Makefile
	$(CPP) -c $<

lz4.o: ../core/lz4.c $(HEADER_FILES) ./Makefile
	$(CC) -c $<

vgstone.o: ../core/vgstone.c $(HEADER_FILES) ./Makefile
	$(CC) -c $<

vgs0math.o: ../core/vgs0math.c $(HEADER_FILES) ./Makefile
	$(CC) -c $<

rom_tndtable.o: ../core/rom_tndtable.c
	${CC} -c $<

emu76489.o: ../core/emu76489.c
	${CC} -c $<
