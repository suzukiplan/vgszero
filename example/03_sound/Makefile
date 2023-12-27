PROJECT = sound
BMP2CHR = ../../tools/bmp2chr/bmp2chr
MAKEROM = ../../tools/makerom/makerom
MAKEBGM = ../../tools/makebgm/makebgm
MAKESE = ../../tools/makese/makese
VGSMML = ../../tools/vgsmml/vgsmml
SDCC_FLAGS = -mz80
SDCC_FLAGS += --opt-code-size
SDCC_FLAGS += --code-loc 0x100
SDCC_FLAGS += --xram-loc 0xC000 
SDCC_FLAGS += --xram-size 0x4000 
SDCC_FLAGS += --no-std-crt0
SDCC_FLAGS += -Wl../../lib/sdcc/vgs0.lib

all: tools ${PROJECT}.rom bgm.dat se.dat
	cd ../../src/sdl2 && make
	../../src/sdl2/vgs0 -b bgm.dat -s se.dat ${PROJECT}.rom

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

tools:
	cd ../../tools && make

${PROJECT}.rom: program.bin font.chr
	${MAKEROM} ${PROJECT}.rom program.bin font.chr

program.bin: program.c ../../lib/sdcc/vgs0.lib ../../lib/sdcc/vgs0lib.h
	sdcc ${SDCC_FLAGS} program.c
	makebin -s 16384 program.ihx program.bin

font.chr: font.bmp
	${BMP2CHR} font.bmp font.chr

bgm.dat: song1.bgm song2.bgm song3.bgm
	${MAKEBGM} bgm.dat song1.bgm song2.bgm song3.bgm

song1.bgm: song1.mml
	${VGSMML} $< $@

song2.bgm: song2.mml
	${VGSMML} $< $@

song3.bgm: song3.mml
	${VGSMML} $< $@

se.dat: se_move.wav se_enter.wav
	${MAKESE} se.dat se_move.wav se_enter.wav
