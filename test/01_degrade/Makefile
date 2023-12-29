all:
	cd ../../src/test && make
	cd ../../example/04_heavy && make build
	../../src/test/vgs0test -f 600 ../../example/04_heavy/game.pkg
	diff expect_ram.bin result_ram.bin
	diff expect_vram.bin result_vram.bin
