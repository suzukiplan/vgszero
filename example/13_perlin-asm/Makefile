BMP2CHR = ../../tools/bmp2chr/bmp2chr
MAKEROM = ../../tools/makerom/makerom
MAKEBGM = ../../tools/makebgm/makebgm
MAKESE = ../../tools/makese/makese
VGSMML = ../../tools/vgsmml/vgsmml
MAKEPKG = ../../tools/makepkg/makepkg
ASM = ../../tools/vgsasm/vgsasm -v

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

game.pkg: program.rom
	${MAKEPKG} -o game.pkg -r program.rom

program.rom: image.chr program.bin
	${MAKEROM} program.rom program.bin image.chr

image.chr: image.bmp
	${BMP2CHR} image.bmp image.chr palette.bin

program.bin: program.asm
	${ASM} program.asm
