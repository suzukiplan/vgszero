BMP2CHR = ../../tools/bmp2chr/bmp2chr
MAKEROM = ../../tools/makerom/makerom
SDCC_FLAGS = -mz80
SDCC_FLAGS += --opt-code-size
SDCC_FLAGS += --code-loc 0x100
SDCC_FLAGS += --no-std-crt0
SDCC_FLAGS += -Wl../../lib/sdcc/vgs0.lib

all: tools hello.rom
	cd ../../src/sdl2 && make
	../../src/sdl2/vgs0 hello.rom

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
	rm -f *.rom

tools:
	cd ../../tools && make

hello.rom: hello.bin font.chr
	${MAKEROM} hello.rom hello.bin font.chr

hello.bin: hello.c ../../lib/sdcc/vgs0.lib ../../lib/sdcc/vgs0lib.h
	sdcc ${SDCC_FLAGS} hello.c
	makebin -s 16384 hello.ihx hello.bin

font.chr: font.bmp
	${BMP2CHR} font.bmp font.chr
