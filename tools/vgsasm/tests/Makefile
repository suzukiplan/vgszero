CPP = g++ -std=c++17 -g
DEPS = common.hpp ../src/*
TEST_MODULES =\
	assignment \
	binary \
	bracket_to_address \
	bracket_eliminate \
	decimal \
	define_parse \
	define_replace \
	enum \
	formulas_evaluate \
	formulas_evaluate_array \
	increment \
	label_parse \
	label_parse_jump \
	label_extract_anonymous \
	literal \
	nametable \
	numeric \
	offset_parse \
	offset_replace \
	operand \
	org

all:
	make execute
	@make execute >result.txt
	@echo Normal case: `cat result.txt | grep -o -c "^N:"`
	@echo Error case: `cat result.txt | grep -o -c "^E:"`
	@echo Total: `cat result.txt | grep -o -c "^[NE]:"`

execute: ${TEST_MODULES}
	./assignment
	./binary
	./bracket_to_address
	./bracket_eliminate
	./decimal
	./define_parse
	./define_replace
	./enum
	./formulas_evaluate
	./formulas_evaluate_array
	./increment
	./label_parse
	./label_parse_jump
	./label_extract_anonymous
	./literal
	./nametable
	./numeric
	./offset_parse
	./offset_replace
	./operand
	./org

single: org
	./org

clean:
	rm -f ${TEST_MODULES}

assignment: assignment.cpp ${DEPS}
	${CPP} $< -o $@

binary: binary.cpp ${DEPS}
	${CPP} $< -o $@

bracket_to_address: bracket_to_address.cpp ${DEPS}
	${CPP} $< -o $@

bracket_eliminate: bracket_eliminate.cpp ${DEPS}
	${CPP} $< -o $@

decimal: decimal.cpp ${DEPS}
	${CPP} $< -o $@

define_parse: define_parse.cpp ${DEPS}
	${CPP} $< -o $@

define_replace: define_replace.cpp ${DEPS}
	${CPP} $< -o $@

enum: enum.cpp ${DEPS}
	${CPP} $< -o $@

formulas_evaluate: formulas_evaluate.cpp ${DEPS}
	${CPP} $< -o $@

formulas_evaluate_array: formulas_evaluate_array.cpp ${DEPS}
	${CPP} $< -o $@

increment: increment.cpp ${DEPS}
	${CPP} $< -o $@

label_parse: label_parse.cpp ${DEPS}
	${CPP} $< -o $@

label_parse_jump: label_parse_jump.cpp ${DEPS}
	${CPP} $< -o $@

label_extract_anonymous: label_extract_anonymous.cpp ${DEPS}
	${CPP} $< -o $@

literal: literal.cpp ${DEPS}
	${CPP} $< -o $@

nametable: nametable.cpp ${DEPS}
	${CPP} $< -o $@

numeric: numeric.cpp ${DEPS}
	${CPP} $< -o $@

offset_parse: offset_parse.cpp ${DEPS}
	${CPP} $< -o $@

offset_replace: offset_replace.cpp ${DEPS}
	${CPP} $< -o $@

operand: operand.cpp ${DEPS}
	${CPP} $< -o $@

org: org.cpp ${DEPS}
	${CPP} $< -o $@
