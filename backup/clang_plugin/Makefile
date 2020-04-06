CC=clang++

all: plugin.so

plugin.so: plugin.cpp
	$(CC) `llvm-config --cflags --libs` -std=c++14 -lclang  -shared  $^ -o $@

run: test.c
	clang -cc1 -load ./plugin.so -plugin print-fns $^
