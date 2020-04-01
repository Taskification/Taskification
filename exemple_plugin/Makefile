CC=clang
CFLAGS=`llvm-config --libs --cxxflags` -O3 -Wall -std=c++2a

all: plugin.so

plugin.so: plugin.cpp
	$(CC) -shared $(CFLAGS) -o $@ $^

run: t.c plugin.so
	$(CC) -Xclang -load  -Xclang ./plugin.so -Xclang -add-plugin -Xclang impurewarnings t.c

clean:
	rm -fr plugin.so ./a.out