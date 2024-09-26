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
	make execute-format FILENAME=./example/15_nsf/program.c
	make execute-format FILENAME=./example/16_ptn-plus1/program.c
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

