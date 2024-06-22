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
OBJECTS += nsf.o
OBJECTS += rconv.o
OBJECTS += nes_cpu.o
OBJECTS += nes_bank.o
OBJECTS += nes_mem.o
OBJECTS += nsf2_vectors.o
OBJECTS += nsf2_irq.o
OBJECTS += nes_apu.o
OBJECTS += nes_dmc.o
OBJECTS += nes_vrc6.o
OBJECTS += rom_tndtable.o

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

nsf.o: ../core/nsf/xgm/player/nsf/nsf.cpp
	${CPP} -c $<

rconv.o: ../core/nsf/xgm/devices/Audio/rconv.cpp
	${CPP} -c $<

nes_cpu.o: ../core/nsf/xgm/devices/CPU/nes_cpu.cpp
	${CPP} -c $<

nes_bank.o: ../core/nsf/xgm/devices/Memory/nes_bank.cpp
	${CPP} -c $<

nes_mem.o: ../core/nsf/xgm/devices/Memory/nes_mem.cpp
	${CPP} -c $<

nsf2_vectors.o: ../core/nsf/xgm/devices/Memory/nsf2_vectors.cpp
	${CPP} -c $<

nsf2_irq.o: ../core/nsf/xgm/devices/Misc/nsf2_irq.cpp
	${CPP} -c $<

nes_apu.o: ../core/nsf/xgm/devices/Sound/nes_apu.cpp
	${CPP} -c $<

nes_dmc.o: ../core/nsf/xgm/devices/Sound/nes_dmc.cpp
	${CPP} -c $<

nes_vrc6.o: ../core/nsf/xgm/devices/Sound/nes_vrc6.cpp
	${CPP} -c $<

rom_tndtable.o: ../core/nsf/xgm/devices/Sound/rom_tndtable.c
	${CC} -c $<
