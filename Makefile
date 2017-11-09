CXX=g++
CXXFLAGS=-std=c++11 -Wall

.PHONY: bin all clean test

bin:
	mkdir -p bin

bin/rho: src/rho.cpp
	g++ $(CXXFLAGS) $^ -o $@

clean:
	rm -rf bin

all: bin bin/rho

test: all
	test/run.sh bin test/in test/out
