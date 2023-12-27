all:
	@echo make format .............	execute clang-format

format:
	make execute-format FILENAME=./src/core/vgs0.hpp
	make execute-format FILENAME=./src/core/vdp.hpp
	make execute-format FILENAME=./src/core/vgs0def.h
	make execute-format FILENAME=./src/core/vgsdecv.hpp
	make execute-format FILENAME=./src/core/vgstone.c
	make execute-format FILENAME=./src/core/z80.hpp

execute-format:
	clang-format -style=file < ${FILENAME} > ${FILENAME}.bak
	cat ${FILENAME}.bak > ${FILENAME}
	rm ${FILENAME}.bak
