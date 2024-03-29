PROJECT = bigsprite
BMP2CHR = ../../tools/bmp2chr/bmp2chr
MAKEROM = ../../tools/makerom/makerom
MAKEPKG = ../../tools/makepkg/makepkg
SDCC_FLAGS = -mz80
SDCC_FLAGS += --opt-code-size
SDCC_FLAGS += --code-loc 0x100
SDCC_FLAGS += --xram-loc 0xC000 
SDCC_FLAGS += --xram-size 0x4000 
SDCC_FLAGS += --no-std-crt0
SDCC_FLAGS += -Wl../../lib/sdcc/vgs0.lib
IMAGES = image00.chr
IMAGES += image01.chr
IMAGES += image02.chr
IMAGES += image03.chr
IMAGES += image04.chr
IMAGES += image05.chr
IMAGES += image06.chr
IMAGES += image07.chr
IMAGES += image08.chr
IMAGES += image09.chr
IMAGES += image10.chr
IMAGES += image11.chr
IMAGES += image12.chr
IMAGES += image13.chr
IMAGES += image14.chr
IMAGES += image15.chr

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

game.pkg: ${PROJECT}.rom
	${MAKEPKG} -o game.pkg -r ${PROJECT}.rom

${PROJECT}.rom: ${IMAGES} program.bin
	${MAKEROM} ${PROJECT}.rom program.bin ${IMAGES}

program.bin: program.c palette.h ../../lib/sdcc/vgs0.lib ../../lib/sdcc/vgs0lib.h
	sdcc ${SDCC_FLAGS} program.c
	makebin -s 16384 program.ihx program.bin

.SUFFIXES: .chr .bmp

%.chr: %.bmp
	${BMP2CHR} $< $@ palette.h
