PROJECT = save
BMP2CHR = ../../tools/bmp2chr/bmp2chr
MAKEROM = ../../tools/makerom/makerom
MAKEPKG = ../../tools/makepkg/makepkg
MAKEBGM = ../../tools/makebgm/makebgm
VGSMML = ../../tools/vgsmml/vgsmml
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

tools:
	cd ../../tools && make

game.pkg: ${PROJECT}.rom bgm.dat
	${MAKEPKG} -o game.pkg -r ${PROJECT}.rom -b bgm.dat

${PROJECT}.rom: program.bin font.chr
	${MAKEROM} ${PROJECT}.rom program.bin font.chr

program.bin: program.c ../../lib/sdcc/vgs0.lib ../../lib/sdcc/vgs0lib.h
	sdcc ${SDCC_FLAGS} program.c
	makebin -s 16384 program.ihx program.bin

font.chr: font.bmp
	${BMP2CHR} font.bmp font.chr

bgm.dat: bgm.mml
	${VGSMML} bgm.mml bgm.bin
	${MAKEBGM} bgm.dat bgm.bin
