CPPFLAGS := ${CPPFLAGS} -Wall -Ivendor -Isrc -Itest -g
# TODO: Figure out how Make determines the C++ compiler for implicit rules.
CPP := g++

SRC := src/*.h

all: test_sm test_eh

test_sm: test/StateManager.out
	test/StateManager.out

test_eh: test/EventHandler.out
	test/EventHandler.out

test/StateManager.out: ${SRC} test/*.h test/StateManager.cpp test/main.o
	${CPP} ${CPPFLAGS} -o $@ test/StateManager.cpp test/main.o

test/EventHandler.out: ${SRC} test/*.h test/EventHandler.cpp test/main.o
	${CPP} ${CPPFLAGS} -o $@ test/EventHandler.cpp test/main.o

clean:
	rm -f */*.o */*/*.o

.PHONY: all test clean
