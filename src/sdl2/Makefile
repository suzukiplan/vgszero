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
HEADER_FILES += ./*.h

OBJECTS = vgszero.o
OBJECTS += BufferQueue.o
OBJECTS += lz4.o
OBJECTS += vgstone.o

all: vgszero

clean:
	rm -f $(OBJECTS)
	rm -f vgszero 

vgszero: $(OBJECTS)
	$(CPP) -o vgszero $(OBJECTS) -L/usr/local/lib -lSDL2

BufferQueue.o: BufferQueue.cpp $(HEADER_FILES) ./Makefile
	$(CPP) -c $<

vgszero.o: vgszero.cpp $(HEADER_FILES) ./Makefile
	$(CPP) -c $<

lz4.o: ../core/lz4.c $(HEADER_FILES) ./Makefile
	$(CC) -c $<

vgstone.o: ../core/vgstone.c $(HEADER_FILES) ./Makefile
	$(CC) -c $<
