CC=clang++
CFLAGS=`llvm-config --libs --cxxflags` -O3 -Wall -std=c++17
LDFLAGS= -lclang
SHLIB=libplugin.so
src=plugin1.cpp
test=test1.c
argplugin= -plugin print-callexpr 

all: $(SHLIB)

$(SHLIB): $(src)
	$(CC) $(CFLAGS) $(LDFLAGS) -shared  $^ -o $@  -fPIC

run:$(test)
	clang -cc1 -load ./$(SHLIB) $(argplugin) $^

clean:
	rm -rf *.so .plugin.cpp.* 
