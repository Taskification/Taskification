CC=mpicc
CFLAGS=-O3 -g
LDFLAGS=-lpthread -lffi -ldl

all: libmpiacc.so test perf fibo perf_pure Ourtest simple

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

perf_pure: perf_pure.c libmpiacc.so
	$(CC) $(CFLAGS) -rdynamic perf_pure.c -o $@ -Wl,-rpath,$(PWD) -L. -I. -lmpiacc

Ourtest: Ourtest.c libmpiacc.so
	$(CC) $(CFLAGS) -rdynamic Ourtest.c -o $@ -Wl,-rpath,$(PWD) -L. -I. -lmpiacc

clean:
	rm ./libmpiacc.so ./test ./perf ./fibo ./Ourtest ./perf_pure ./simple
