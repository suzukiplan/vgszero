BMP2CHR = ../../tools/bmp2chr/bmp2chr
MAKEROM = ../../tools/makerom/makerom
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

tools:
	cd ../../tools && make

game.pkg: program.rom
	${MAKEPKG} -o game.pkg -r program.rom

program.rom: font.chr program.bin
	${MAKEROM} program.rom program.bin palette.bin font.chr

font.chr: font.bmp
	${BMP2CHR} font.bmp font.chr palette.bin

program.bin: program.asm
	${ASM} program.asm
