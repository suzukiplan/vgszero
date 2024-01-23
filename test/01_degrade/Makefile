CHKDIF = ../../tools/chkdif/chkdif

all:
	which sdcc
	cd ../../src/test && make
	cd ../../tools && make
	cd ../../example/04_heavy && make build PATH=${PATH}
	../../src/test/vgs0test -f 600 ../../example/04_heavy/game.pkg
	${CHKDIF} expect_vram.bin result_vram.bin
