all:
	@echo make format .............	execute clang-format

format:
	make execute-format FILENAME=./src/core/vgs0.hpp
	make execute-format FILENAME=./src/core/vdp.hpp
	make execute-format FILENAME=./src/core/vgs0def.h
	make execute-format FILENAME=./src/core/vgsdecv.hpp
	make execute-format FILENAME=./src/core/vgstone.c
	make execute-format FILENAME=./src/core/z80.hpp
	make execute-format FILENAME=./src/rpizero/src/kernel.cpp
	make execute-format FILENAME=./src/rpizero/src/kernel.h
	make execute-format FILENAME=./src/rpizero/src/main.cpp
	make execute-format FILENAME=./src/rpizero/src/splash.c
	make execute-format FILENAME=./src/rpizero/src/std.c
	make execute-format FILENAME=./src/sdl2/vgs0sdl2.cpp
	make execute-format FILENAME=./tools/bmp2chr/bmp2chr.c
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
	make execute-format FILENAME=./lib/sdcc/vgs0lib.h

execute-format:
	clang-format -style=file < ${FILENAME} > ${FILENAME}.bak
	cat ${FILENAME}.bak > ${FILENAME}
	rm ${FILENAME}.bak
