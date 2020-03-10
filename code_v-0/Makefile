CC=clang++

all: Task.so

Task.so: Task.cpp
	$(CC) `llvm-config --libs --cxxflags ` -std=c++17 -lclang  -shared  $^ -o $@  -fPIC

run: fonction_a_tester.c
	clang -cc1 -load ./Task.so -plugin -task-plugin $^

clean: 
	rm *.so