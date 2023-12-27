all:
	cd src && git submodule update --init circle
	cp src/Config.mk src/circle
	cp -p ../core/lz4.c src
	cp -p ../core/vgstone.c src
	cd src/circle && ./makeall --nosample
	cd src/circle/addon/fatfs && make
	cd src/circle/addon/SDCard && make
	cd src/circle/addon/linux && make
	cd src/circle/addon/vc4 && ./makeall --nosample
	cd src && make -f Build.mk
	cp src/kernel8.img ../../image

clean:
	cd src && make -f Build.mk clean

cleanall:
	cd src/circle && ./makeall clean
	cd src/circle/addon/fatfs && make clean
	cd src/circle/addon/SDCard && make clean
	cd src/circle/addon/linux && make clean
	cd src/circle/addon/vc4 && ./makeall clean
	make clean
