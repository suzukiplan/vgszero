all:
	cd src && git submodule update --init circle
	cp src/Config.mk src/circle
	cd src/circle && ./makeall --nosample
	cd src && make -f Build.mk
	cp src/kernel8.img image/kernel8.img

clean:
	cd src && make -f Build.mk clean

cleanall:
	cd src/circle && ./makeall clean
	make clean
