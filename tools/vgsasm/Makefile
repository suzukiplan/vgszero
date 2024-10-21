CPP = g++
CPPFLAGS = -std=c++17 -g
HEADERS = src/common.h
OBJECTS = \
	macro.o \
	binary.o \
	bracket.o \
	define.o \
	file.o \
	formulas.o \
	increment.o \
	label.o \
	line.o \
	literal.o \
	mnemonic.o \
	mnemonic_data.o \
	mnemonic_call.o \
	mnemonic_jump.o \
	mnemonic_io.o \
	mnemonic_shift.o \
	mnemonic_load.o \
	mnemonic_calc.o \
	mnemonic_incdec.o \
	mnemonic_ex.o \
	mnemonic_stack.o \
	mnemonic_bit.o \
	mnemonic_vgs.o \
	numeric.o \
	operand.o \
	sizeof.o \
	offset.o \
	struct.o \
	org.o \
	main.o

all: vgsasm

check: vgsasm
	./vgsasm -o all.bin test/all.asm
	z88dk.z88dk-dis all.bin >test/all_disassemble.asm
	diff test/all_expect.asm test/all_disassemble.asm

update:
	cp test/all_disassemble.asm test/all_expect.asm

clean:
	rm -f ${OBJECTS}
	rm -f vgsasm

vgsasm: ${OBJECTS}
	${CPP} ${CPPFLAGS} -o vgsasm ${OBJECTS}

macro.o: src/macro.cpp ${HEADERS} src/mnemonic.h
	${CPP} ${CPPFLAGS} -c $< -o $@

literal.o: src/literal.cpp ${HEADERS} src/mnemonic.h
	${CPP} ${CPPFLAGS} -c $< -o $@

increment.o: src/increment.cpp ${HEADERS}
	${CPP} ${CPPFLAGS} -c $< -o $@

binary.o: src/binary.cpp ${HEADERS}
	${CPP} ${CPPFLAGS} -c $< -o $@

bracket.o: src/bracket.cpp ${HEADERS}
	${CPP} ${CPPFLAGS} -c $< -o $@

define.o: src/define.cpp ${HEADERS}
	${CPP} ${CPPFLAGS} -c $< -o $@

file.o: src/file.cpp ${HEADERS}
	${CPP} ${CPPFLAGS} -c $< -o $@

formulas.o: src/formulas.cpp ${HEADERS}
	${CPP} ${CPPFLAGS} -c $< -o $@

label.o: src/label.cpp ${HEADERS}
	${CPP} ${CPPFLAGS} -c $< -o $@

line.o: src/line.cpp ${HEADERS}
	${CPP} ${CPPFLAGS} -c $< -o $@

mnemonic.o: src/mnemonic.cpp ${HEADERS} src/mnemonic.h
	${CPP} ${CPPFLAGS} -c $< -o $@

mnemonic_data.o: src/mnemonic_data.cpp ${HEADERS} src/mnemonic.h
	${CPP} ${CPPFLAGS} -c $< -o $@

mnemonic_jump.o: src/mnemonic_jump.cpp ${HEADERS} src/mnemonic.h
	${CPP} ${CPPFLAGS} -c $< -o $@

mnemonic_call.o: src/mnemonic_call.cpp ${HEADERS} src/mnemonic.h
	${CPP} ${CPPFLAGS} -c $< -o $@

mnemonic_io.o: src/mnemonic_io.cpp ${HEADERS} src/mnemonic.h
	${CPP} ${CPPFLAGS} -c $< -o $@

mnemonic_shift.o: src/mnemonic_shift.cpp ${HEADERS} src/mnemonic.h
	${CPP} ${CPPFLAGS} -c $< -o $@

mnemonic_load.o: src/mnemonic_load.cpp ${HEADERS} src/mnemonic.h
	${CPP} ${CPPFLAGS} -c $< -o $@

mnemonic_calc.o: src/mnemonic_calc.cpp ${HEADERS} src/mnemonic.h
	${CPP} ${CPPFLAGS} -c $< -o $@

mnemonic_incdec.o: src/mnemonic_incdec.cpp ${HEADERS} src/mnemonic.h
	${CPP} ${CPPFLAGS} -c $< -o $@

mnemonic_ex.o: src/mnemonic_ex.cpp ${HEADERS} src/mnemonic.h
	${CPP} ${CPPFLAGS} -c $< -o $@

mnemonic_stack.o: src/mnemonic_stack.cpp ${HEADERS} src/mnemonic.h
	${CPP} ${CPPFLAGS} -c $< -o $@

mnemonic_bit.o: src/mnemonic_bit.cpp ${HEADERS} src/mnemonic.h
	${CPP} ${CPPFLAGS} -c $< -o $@

mnemonic_vgs.o: src/mnemonic_vgs.cpp ${HEADERS} src/mnemonic.h
	${CPP} ${CPPFLAGS} -c $< -o $@

numeric.o: src/numeric.cpp ${HEADERS}
	${CPP} ${CPPFLAGS} -c $< -o $@

operand.o: src/operand.cpp ${HEADERS}
	${CPP} ${CPPFLAGS} -c $< -o $@

sizeof.o: src/sizeof.cpp ${HEADERS}
	${CPP} ${CPPFLAGS} -c $< -o $@

offset.o: src/offset.cpp ${HEADERS}
	${CPP} ${CPPFLAGS} -c $< -o $@

struct.o: src/struct.cpp ${HEADERS}
	${CPP} ${CPPFLAGS} -c $< -o $@

org.o: src/org.cpp ${HEADERS}
	${CPP} ${CPPFLAGS} -c $< -o $@

main.o: src/main.cpp ${HEADERS}
	${CPP} ${CPPFLAGS} -c $< -o $@
