BMP2CHR = ../../tools/bmp2chr/bmp2chr
MAKEROM = ../../tools/makerom/makerom
MAKEBGM = ../../tools/makebgm/makebgm
MAKESE = ../../tools/makese/makese
VGSMML = ../../tools/vgsmml/vgsmml
MAKEPKG = ../../tools/makepkg/makepkg
ASM = ../../tools/vgsasm/vgsasm -v
IMAGES = image00.chr \
	image01.chr \
	image02.chr \
	image03.chr \
	image04.chr \
	image05.chr \
	image06.chr \
	image07.chr \
	image08.chr \
	image09.chr \
	image10.chr \
	image11.chr \
	image12.chr \
	image13.chr \
	image14.chr \
	image15.chr

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

program.rom: ${IMAGES} program.bin
	${MAKEROM} program.rom program.bin ${IMAGES}

program.bin: program.asm
	${ASM} $<

.SUFFIXES: .chr .bmp

%.chr: %.bmp
	${BMP2CHR} $< $@ palette.bin
