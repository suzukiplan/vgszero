TEST_MODULES = assignment binary

all: ${TEST_MODULES}
	./assignment
	./binary

clean:
	rm -f ${TEST_MODULES}

assignment: assignment.cpp common.hpp ../src/*
	g++ -std=c++17 -g $< -o $@

binary: binary.cpp common.hpp ../src/*
	g++ -std=c++17 -g $< -o $@
