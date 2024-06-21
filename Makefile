all:
	@echo make format .............	execute clang-format
	@echo make build .............. Build API, SDL2, RPI, Hello
	@echo make clean .............. Clean API, SDL2, RPI, Hello
	@echo make tests .............. execute all tests

format:
	make execute-format FILENAME=./src/core/vgs0.hpp
	make execute-format FILENAME=./src/core/vdp.hpp
	make execute-format FILENAME=./src/core/vgs0def.h
	make execute-format FILENAME=./src/core/vgsdecv.hpp
	make execute-format FILENAME=./src/core/vgstone.c
	make execute-format FILENAME=./src/core/z80.hpp
	make execute-format FILENAME=./src/core/vgs0math.c
	make execute-format FILENAME=./src/core/perlinnoise.hpp


	make execute-format FILENAME=./src/core/nsf/cli.cpp
	make execute-format FILENAME=./src/core/nsf/nsfplay.hpp
	make execute-format FILENAME=./src/core/nsf/xgm/browser/browser.h
	make execute-format FILENAME=./src/core/nsf/xgm/version.h
	make execute-format FILENAME=./src/core/nsf/xgm/player/all.h
	make execute-format FILENAME=./src/core/nsf/xgm/player/player.h
	make execute-format FILENAME=./src/core/nsf/xgm/player/midi_interface.h
	make execute-format FILENAME=./src/core/nsf/xgm/player/soundData.h
	make execute-format FILENAME=./src/core/nsf/xgm/player/nsf/pls/sstream.h
	make execute-format FILENAME=./src/core/nsf/xgm/player/nsf/pls/sstream.cpp
	make execute-format FILENAME=./src/core/nsf/xgm/player/nsf/pls/ppls.cpp
	make execute-format FILENAME=./src/core/nsf/xgm/player/nsf/pls/ppls.h
	make execute-format FILENAME=./src/core/nsf/xgm/player/nsf/nsf.cpp
	make execute-format FILENAME=./src/core/nsf/xgm/player/nsf/nsf.h
	make execute-format FILENAME=./src/core/nsf/xgm/devices/CPU/nes_cpu.h
	make execute-format FILENAME=./src/core/nsf/xgm/devices/CPU/nes_cpu.cpp
	make execute-format FILENAME=./src/core/nsf/xgm/devices/CPU/km6502/km65c02m.h
	make execute-format FILENAME=./src/core/nsf/xgm/devices/CPU/km6502/km6502ct.h
	make execute-format FILENAME=./src/core/nsf/xgm/devices/CPU/km6502/km6502ot.h
	make execute-format FILENAME=./src/core/nsf/xgm/devices/CPU/km6502/km6280m.h
	make execute-format FILENAME=./src/core/nsf/xgm/devices/CPU/km6502/km6502m.h
	make execute-format FILENAME=./src/core/nsf/xgm/devices/CPU/km6502/kmconfig.h
	make execute-format FILENAME=./src/core/nsf/xgm/devices/CPU/km6502/km6502ex.h
	make execute-format FILENAME=./src/core/nsf/xgm/devices/CPU/km6502/km6502.h
	make execute-format FILENAME=./src/core/nsf/xgm/devices/CPU/km6502/km6280.h
	make execute-format FILENAME=./src/core/nsf/xgm/devices/CPU/km6502/km6502cd.h
	make execute-format FILENAME=./src/core/nsf/xgm/devices/CPU/km6502/km65c02.h
	make execute-format FILENAME=./src/core/nsf/xgm/devices/CPU/km6502/km6502ft.h
	make execute-format FILENAME=./src/core/nsf/xgm/devices/all.h
	make execute-format FILENAME=./src/core/nsf/xgm/devices/Sound/nes_n106.cpp
	make execute-format FILENAME=./src/core/nsf/xgm/devices/Sound/nes_n106.h
	make execute-format FILENAME=./src/core/nsf/xgm/devices/Sound/nes_fds.h
	make execute-format FILENAME=./src/core/nsf/xgm/devices/Sound/nes_fds.cpp
	make execute-format FILENAME=./src/core/nsf/xgm/devices/Sound/nes_mmc5.cpp
	make execute-format FILENAME=./src/core/nsf/xgm/devices/Sound/nes_apu.h
	make execute-format FILENAME=./src/core/nsf/xgm/devices/Sound/nes_fme7.cpp
	make execute-format FILENAME=./src/core/nsf/xgm/devices/Sound/nes_vrc6.h
	make execute-format FILENAME=./src/core/nsf/xgm/devices/Sound/nes_vrc6.cpp
	make execute-format FILENAME=./src/core/nsf/xgm/devices/Sound/legacy/emutypes.h
	make execute-format FILENAME=./src/core/nsf/xgm/devices/Sound/legacy/emu2149.c
	make execute-format FILENAME=./src/core/nsf/xgm/devices/Sound/legacy/emu2212.h
	make execute-format FILENAME=./src/core/nsf/xgm/devices/Sound/legacy/281btone.h
	make execute-format FILENAME=./src/core/nsf/xgm/devices/Sound/legacy/emu2149.h
	make execute-format FILENAME=./src/core/nsf/xgm/devices/Sound/legacy/281btone_plgdavid.h
	make execute-format FILENAME=./src/core/nsf/xgm/devices/Sound/legacy/emu2212.c
	make execute-format FILENAME=./src/core/nsf/xgm/devices/Sound/nes_apu.cpp
	make execute-format FILENAME=./src/core/nsf/xgm/devices/Sound/nes_mmc5.h
	make execute-format FILENAME=./src/core/nsf/xgm/devices/Sound/nes_dmc.cpp
	make execute-format FILENAME=./src/core/nsf/xgm/devices/Sound/nes_dmc.h
	make execute-format FILENAME=./src/core/nsf/xgm/devices/Sound/nes_fme7.h
	make execute-format FILENAME=./src/core/nsf/xgm/devices/Memory/nsf2_vectors.cpp
	make execute-format FILENAME=./src/core/nsf/xgm/devices/Memory/nes_bank.cpp
	make execute-format FILENAME=./src/core/nsf/xgm/devices/Memory/ram64k.cpp
	make execute-format FILENAME=./src/core/nsf/xgm/devices/Memory/nsf2_vectors.h
	make execute-format FILENAME=./src/core/nsf/xgm/devices/Memory/nes_bank.h
	make execute-format FILENAME=./src/core/nsf/xgm/devices/Memory/ram64k.h
	make execute-format FILENAME=./src/core/nsf/xgm/devices/Memory/nes_mem.cpp
	make execute-format FILENAME=./src/core/nsf/xgm/devices/Memory/nes_mem.h
	make execute-format FILENAME=./src/core/nsf/xgm/devices/device.h
	make execute-format FILENAME=./src/core/nsf/xgm/devices/Misc/block.h
	make execute-format FILENAME=./src/core/nsf/xgm/devices/Misc/nes_detect.h
	make execute-format FILENAME=./src/core/nsf/xgm/devices/Misc/nes_detect.cpp
	make execute-format FILENAME=./src/core/nsf/xgm/devices/Misc/detect.cpp
	make execute-format FILENAME=./src/core/nsf/xgm/devices/Misc/detect.h
	make execute-format FILENAME=./src/core/nsf/xgm/devices/Misc/nsf2_irq.cpp
	make execute-format FILENAME=./src/core/nsf/xgm/devices/Misc/nsf2_irq.h
	make execute-format FILENAME=./src/core/nsf/xgm/devices/Misc/log_cpu.h
	make execute-format FILENAME=./src/core/nsf/xgm/devices/Misc/log_cpu.cpp
	make execute-format FILENAME=./src/core/nsf/xgm/devices/Audio/MedianFilter.h
	make execute-format FILENAME=./src/core/nsf/xgm/devices/Audio/mixer.h
	make execute-format FILENAME=./src/core/nsf/xgm/devices/Audio/MedianFilter.cpp
	make execute-format FILENAME=./src/core/nsf/xgm/devices/Audio/fader.h
	make execute-format FILENAME=./src/core/nsf/xgm/devices/Audio/echo.cpp
	make execute-format FILENAME=./src/core/nsf/xgm/devices/Audio/filter.cpp
	make execute-format FILENAME=./src/core/nsf/xgm/devices/Audio/rconv.h
	make execute-format FILENAME=./src/core/nsf/xgm/devices/Audio/echo.h
	make execute-format FILENAME=./src/core/nsf/xgm/devices/Audio/rconv.cpp
	make execute-format FILENAME=./src/core/nsf/xgm/devices/Audio/amplifier.h
	make execute-format FILENAME=./src/core/nsf/xgm/devices/Audio/filter.h
	make execute-format FILENAME=./src/core/nsf/xgm/devices/devinfo.h
	make execute-format FILENAME=./src/core/nsf/xgm/xtypes.h
	make execute-format FILENAME=./src/core/nsf/xgm/xgm.h
	make execute-format FILENAME=./src/core/nsf/xgm/utils/tagctrl.h
	make execute-format FILENAME=./src/core/nsf/xgm/debugout.h
	make execute-format FILENAME=./src/rpizero/src/kernel.cpp
	make execute-format FILENAME=./src/rpizero/src/kernel.h
	make execute-format FILENAME=./src/rpizero/src/main.cpp
	make execute-format FILENAME=./src/rpizero/src/splash.c
	make execute-format FILENAME=./src/rpizero/src/std.c
	make execute-format FILENAME=./src/sdl2/vgs0sdl2.cpp
	make execute-format FILENAME=./src/test/vgs0test.cpp
	make execute-format FILENAME=./tools/bmp2chr/bmp2chr.c
	make execute-format FILENAME=./tools/makepkg/makepkg.cpp
	make execute-format FILENAME=./tools/makebgm/makebgm.cpp
	make execute-format FILENAME=./tools/makerom/makerom.c
	make execute-format FILENAME=./tools/makese/makese.cpp
	make execute-format FILENAME=./tools/vgsmml/src/cli.c
	make execute-format FILENAME=./tools/vgsmml/src/vgsdec_internal.h
	make execute-format FILENAME=./tools/vgsmml/src/vgsdec.h
	make execute-format FILENAME=./tools/vgsmml/src/vgsftv.cpp
	make execute-format FILENAME=./tools/vgsmml/src/vgsmml.c
	make execute-format FILENAME=./tools/vgsmml/src/vgsmml.h
	make execute-format FILENAME=./example/01_hello/program.c
	make execute-format FILENAME=./example/02_global/program.c
	make execute-format FILENAME=./example/02_global/global.h
	make execute-format FILENAME=./example/03_sound/program.c
	make execute-format FILENAME=./example/04_heavy/program.c
	make execute-format FILENAME=./example/05_sprite256/program.c
	make execute-format FILENAME=./example/06_save/program.c
	make execute-format FILENAME=./example/07_palette/program.c
	make execute-format FILENAME=./example/08_map-scroll/program.c
	make execute-format FILENAME=./example/09_joypad/program.c
	make execute-format FILENAME=./example/10_chr720/program.c
	make execute-format FILENAME=./example/11_bigsprite/program.c
	make execute-format FILENAME=./example/12_angle/program.c
	make execute-format FILENAME=./example/13_perlin/program.c
	make execute-format FILENAME=./example/14_1024ptn/program.c
	make execute-format FILENAME=./lib/sdcc/vgs0lib.h

execute-format:
	clang-format -style=file < ${FILENAME} > ${FILENAME}.bak
	cat ${FILENAME}.bak > ${FILENAME}
	rm ${FILENAME}.bak

build:
	cd lib/sdcc && make
	cd src/sdl2 && make
	cd src/rpizero2 && make
	cd example && make
	cd example/01_hello && make
	cp -p example/01_hello/game.pkg image

clean:
	cd lib/sdcc && make clean
	cd src/sdl2 && make clean
	cd src/rpizero2 && make clean
	cd example/01_hello && make clean

ci:
	make sdcc-4.1.0-amd64-unknown-linux2.5.tar.bz2
	make ./sdcc/bin/sdcc
	cd test && make PATH="`pwd`/../sdcc/bin:${PATH}"

./sdcc/bin/sdcc:
	tar xvf sdcc-4.1.0-amd64-unknown-linux2.5.tar.bz2

sdcc-4.1.0-amd64-unknown-linux2.5.tar.bz2:
	wget https://sourceforge.net/projects/sdcc/files/sdcc-linux-amd64/4.1.0/sdcc-4.1.0-amd64-unknown-linux2.5.tar.bz2

