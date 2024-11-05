BMP2CHR = ../../tools/bmp2chr/bmp2chr
MAKEROM = ../../tools/makerom/makerom
MAKEBGM = ../../tools/makebgm/makebgm
MAKESE = ../../tools/makese/makese
VGSMML = ../../tools/vgsmml/vgsmml
MAKEPKG = ../../tools/makepkg/makepkg
ASM = ../../tools/vgsasm/vgsasm

all: build
	cd ../../src/sdl2 && make
	../../src/sdl2/vgs0 -d game.pkg

build: tools game.pkg

clean:
	rm -f *.bin
	rm -f *.o
	rm -f *.chr
	rm -f *.rom
	rm -f game.pkg
	rm -f bgm.dat
	rm -f se.dat

tools:
	cd ../../tools && make

game.pkg: program.rom bgm.dat se.dat
	${MAKEPKG} -o game.pkg -r program.rom -b bgm.dat -s se.dat

program.rom: font.chr program.bin
	${MAKEROM} program.rom program.bin font.chr

font.chr: font.bmp
	${BMP2CHR} font.bmp font.chr palette.bin

program.bin: program.asm
	${ASM} program.asm

bgm.dat: bgm.nsf
	${MAKEBGM} bgm.dat bgm.nsf

se.dat: se_move.wav se_enter.wav
	${MAKESE} se.dat se_move.wav se_enter.wav

