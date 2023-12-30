PROJECT = map-scroll
BMP2CHR = ../../tools/bmp2chr/bmp2chr
CSV2BIN = ../../tools/csv2bin/csv2bin
MAKEROM = ../../tools/makerom/makerom
MAKEPKG = ../../tools/makepkg/makepkg
SDCC_FLAGS = -mz80
SDCC_FLAGS += --opt-code-size
SDCC_FLAGS += --code-loc 0x100
SDCC_FLAGS += --xram-loc 0xC000 
SDCC_FLAGS += --xram-size 0x4000 
SDCC_FLAGS += --no-std-crt0
SDCC_FLAGS += -Wl../../lib/sdcc/vgs0.lib

all: build
	cd ../../src/sdl2 && make
	../../src/sdl2/vgs0 game.pkg

build: tools game.pkg

clean:
	rm -f *.rel
	rm -f *.chr
	rm -f *.lst
	rm -f *.sym
	rm -f *.asm
	rm -f *.ihx
	rm -f *.map
	rm -f *.bin
	rm -f *.lk
	rm -f *.noi
	rm -f ${PROJECT}.rom
	rm -f bgm.dat
	rm -f se.dat
	rm -f game.pkg
	rm -f mapdata.bin

tools:
	cd ../../tools && make

game.pkg: ${PROJECT}.rom
	${MAKEPKG} -o game.pkg -r ${PROJECT}.rom

${PROJECT}.rom: mapdata.bin image.chr program.bin
	${MAKEROM} ${PROJECT}.rom program.bin image.chr mapdata.bin

image.chr: image.bmp
	${BMP2CHR} image.bmp image.chr palette.h

mapdata.bin: mapdata.csv
	${CSV2BIN} mapdata.csv mapdata.bin

program.bin: program.c palette.h ../../lib/sdcc/vgs0.lib ../../lib/sdcc/vgs0lib.h
	sdcc ${SDCC_FLAGS} program.c
	makebin -s 16384 program.ihx program.bin
