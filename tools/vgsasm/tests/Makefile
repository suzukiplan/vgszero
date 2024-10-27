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
	formulas_evaluate

all: ${TEST_MODULES}
	./assignment
	./binary
	./bracket_to_address
	./bracket_eliminate
	./decimal
	./define_parse
	./define_replace
	./enum
	./formulas_evaluate

single: formulas_evaluate
	./formulas_evaluate

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
