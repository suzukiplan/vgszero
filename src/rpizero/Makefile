all:
	cd src && git submodule update --init circle
	cp src/Config.mk src/circle
	cp -p ../../src/lz4.c src
	cp -p ../../src/vgstone.c src
	cd src/circle && ./makeall --nosample
	cd src/circle/addon/fatfs && make
	cd src/circle/addon/SDCard && make
	cd src/circle/addon/linux && make
	cd src/circle/addon/vc4 && ./makeall --nosample
	cd src && make -f Build.mk
	cp src/kernel.img image/kernel.img

clean:
	cd src && make -f Build.mk clean

cleanall:
	cd src/circle && ./makeall clean
	cd src/circle/addon/fatfs && clean
	cd src/circle/addon/SDCard && make clean
	cd src/circle/addon/linux && make clean
	cd src/circle/addon/vc4 && ./makeall clean
	make clean
