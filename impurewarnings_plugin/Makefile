CC=clang
CFLAGS=`llvm-config --libs --cxxflags` -O3 -Wall -std=c++17

all: plugin.so

plugin.so: plugin.cpp
	$(CC) -lclang -shared $(CFLAGS) -o $@ $^ -fPIC

run: t.c plugin.so
	$(CC) -Xclang -load  -Xclang ./plugin.so -Xclang -add-plugin -Xclang impurewarnings t.c

clean:
	rm -rf  plugin.so ./a.out
