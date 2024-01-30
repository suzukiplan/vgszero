all: chkcfg
	cat ../../image/config.sys
	./chkcfg ../../image/config.sys
	cat test.sys
	./chkcfg test.sys

chkcfg: chkcfg.cpp ../../src/rpizero2/src/config.hpp
	g++ -std=c++11 -DTEST chkcfg.cpp -o chkcfg
