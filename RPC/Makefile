CC=mpicc
CFLAGS=-O3 -g
LDFLAGS=-lpthread -lffi -ldl

all: libmpiacc.so test perf fibo

libmpiacc.so: mpi_accelerator.c
	$(CC) $(CFLAGS) -fpic -shared $^ -o $@ $(LDFLAGS)


fibo: fibo.c libmpiacc.so
	$(CC) $(CFLAGS) -rdynamic fibo.c -o $@ -Wl,-rpath,$(PWD) -L. -I. -lmpiacc



simple: simple.c libmpiacc.so
	$(CC) $(CFLAGS) -rdynamic simple.c -o $@ -Wl,-rpath,$(PWD) -L. -I. -lmpiacc



test: test.c libmpiacc.so
	$(CC) $(CFLAGS) -rdynamic test.c -o $@ -Wl,-rpath,$(PWD) -L. -I. -lmpiacc

perf: perf.c libmpiacc.so
	$(CC) $(CFLAGS) -rdynamic perf.c -o $@ -Wl,-rpath,$(PWD) -L. -I. -lmpiacc

clean:
	rm ./libmpiacc.so ./test ./perf ./fibo
