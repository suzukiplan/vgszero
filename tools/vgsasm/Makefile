all: vgsasm

install: vgsasm
	sudo rm -f /usr/local/bin/vgsasm
	sudo ln -s `pwd`/vgsasm /usr/local/bin/vgsasm

check: vgsasm
	./vgsasm -o all.bin test/all.asm
	z88dk.z88dk-dis all.bin >test/all_disassemble.asm
	diff test/all_expect.asm test/all_disassemble.asm

update:
	cp test/all_disassemble.asm test/all_expect.asm

clean:
	rm -f vgsasm

vgsasm: src/vgsasm.cpp src/*.h src/*.hpp
	g++ -std=c++17 -g -o vgsasm $<
