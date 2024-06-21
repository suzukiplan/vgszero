all:
	cd src && git submodule update --init circle
	cp src/Config.mk src/circle
	cp -p ../core/vgstone.c src
	cp -p ../core/vgs0math.c src
	cp -p ../core/nsf/xgm/player/nsf/nsf.cpp src
	cp -p ../core/nsf/xgm/player/nsf/pls/ppls.cpp src
	cp -p ../core/nsf/xgm/player/nsf/pls/sstream.cpp src
	cp -p ../core/nsf/xgm/devices/Audio/echo.cpp src
	cp -p ../core/nsf/xgm/devices/Audio/filter.cpp src
	cp -p ../core/nsf/xgm/devices/Audio/MedianFilter.cpp src
	cp -p ../core/nsf/xgm/devices/Audio/rconv.cpp src
	cp -p ../core/nsf/xgm/devices/CPU/nes_cpu.cpp src
	cp -p ../core/nsf/xgm/devices/Memory/nes_bank.cpp src
	cp -p ../core/nsf/xgm/devices/Memory/nes_mem.cpp src
	cp -p ../core/nsf/xgm/devices/Memory/nsf2_vectors.cpp src
	cp -p ../core/nsf/xgm/devices/Memory/ram64k.cpp src
	cp -p ../core/nsf/xgm/devices/Misc/detect.cpp src
	cp -p ../core/nsf/xgm/devices/Misc/log_cpu.cpp src
	cp -p ../core/nsf/xgm/devices/Misc/nes_detect.cpp src
	cp -p ../core/nsf/xgm/devices/Misc/nsf2_irq.cpp src
	cp -p ../core/nsf/xgm/devices/Sound/nes_apu.cpp src
	cp -p ../core/nsf/xgm/devices/Sound/nes_dmc.cpp src
	cp -p ../core/nsf/xgm/devices/Sound/nes_fds.cpp src
	cp -p ../core/nsf/xgm/devices/Sound/nes_fme7.cpp src
	cp -p ../core/nsf/xgm/devices/Sound/nes_mmc5.cpp src
	cp -p ../core/nsf/xgm/devices/Sound/nes_n106.cpp src
	cp -p ../core/nsf/xgm/devices/Sound/nes_vrc6.cpp src
	cp -p ../core/nsf/xgm/devices/Sound/legacy/emu2149.c src
	cp -p ../core/nsf/xgm/devices/Sound/legacy/emu2212.c src
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
