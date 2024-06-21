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
OBJECTS += nsf.o
OBJECTS += ppls.o
OBJECTS += sstream.o
OBJECTS += echo.o
OBJECTS += filter.o
OBJECTS += MedianFilter.o
OBJECTS += rconv.o
OBJECTS += nes_cpu.o
OBJECTS += nes_bank.o
OBJECTS += nes_mem.o
OBJECTS += nsf2_vectors.o
OBJECTS += ram64k.o
OBJECTS += detect.o
OBJECTS += nes_detect.o
OBJECTS += nsf2_irq.o
OBJECTS += nes_apu.o
OBJECTS += nes_dmc.o
OBJECTS += nes_fds.o
OBJECTS += nes_fme7.o
OBJECTS += nes_mmc5.o
OBJECTS += nes_n106.o
OBJECTS += nes_vrc6.o
OBJECTS += emu2149.o
OBJECTS += emu2212.o

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

nsf.o: ../core/nsf/xgm/player/nsf/nsf.cpp
	${CPP} -c $<

ppls.o: ../core/nsf/xgm/player/nsf/pls/ppls.cpp
	${CPP} -c $<

sstream.o: ../core/nsf/xgm/player/nsf/pls/sstream.cpp
	${CPP} -c $<

echo.o: ../core/nsf/xgm/devices/Audio/echo.cpp
	${CPP} -c $<

filter.o: ../core/nsf/xgm/devices/Audio/filter.cpp
	${CPP} -c $<

MedianFilter.o: ../core/nsf/xgm/devices/Audio/MedianFilter.cpp
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

ram64k.o: ../core/nsf/xgm/devices/Memory/ram64k.cpp
	${CPP} -c $<

detect.o: ../core/nsf/xgm/devices/Misc/detect.cpp
	${CPP} -c $<

nes_detect.o: ../core/nsf/xgm/devices/Misc/nes_detect.cpp
	${CPP} -c $<

nsf2_irq.o: ../core/nsf/xgm/devices/Misc/nsf2_irq.cpp
	${CPP} -c $<

nes_apu.o: ../core/nsf/xgm/devices/Sound/nes_apu.cpp
	${CPP} -c $<

nes_dmc.o: ../core/nsf/xgm/devices/Sound/nes_dmc.cpp
	${CPP} -c $<

nes_fds.o: ../core/nsf/xgm/devices/Sound/nes_fds.cpp
	${CPP} -c $<

nes_fme7.o: ../core/nsf/xgm/devices/Sound/nes_fme7.cpp
	${CPP} -c $<

nes_mmc5.o: ../core/nsf/xgm/devices/Sound/nes_mmc5.cpp
	${CPP} -c $<

nes_n106.o: ../core/nsf/xgm/devices/Sound/nes_n106.cpp
	${CPP} -c $<

nes_vrc6.o: ../core/nsf/xgm/devices/Sound/nes_vrc6.cpp
	${CPP} -c $<

nes_vrc7.o: ../core/nsf/xgm/devices/Sound/nes_vrc7.cpp
	${CPP} -c $<

emu2149.o: ../core/nsf/xgm/devices/Sound/legacy/emu2149.c
	${CC} -c $<

emu2212.o: ../core/nsf/xgm/devices/Sound/legacy/emu2212.c
	${CC} -c $<

emu2413.o: ../core/nsf/xgm/devices/Sound/legacy/emu2413.c
	${CC} -c $<
