CPPFLAGS := ${CPPFLAGS} -Wall -Ivendor
# TODO: Figure out how Make determines the C++ compiler for implicit rules.
CPP := g++

all: test

test: test/StateManager.out
	test/StateManager.out

test/StateManager.out: export CPPFLAGS := ${CPPFLAGS} -IStateManager/src -Itest
test/StateManager.out: StateManager/src/StateManager.h test/StateManager.o test/*.h test/main.o
	${CPP} ${CPPFLAGS} -o $@ test/StateManager.o test/main.o

clean:
	rm -f */*.o */*/*.o

.PHONY: all test clean
