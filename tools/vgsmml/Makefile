all: vgsmml

vgsmml: src/vgsmml.c src/vgsftv.cpp src/cli.c src/vgsmml.h src/vgsdec.h src/vgsdec_internal.h
	g++ -std=c++17 -c src/vgsftv.cpp
	gcc -c src/vgsmml.c
	gcc -I../../src/core -c src/cli.c
	g++ -o vgsmml vgsftv.o vgsmml.o cli.o

clean:
	rm -f vgsmml
	rm -f *.o

