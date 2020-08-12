CC=gcc
CFLAGS=-fopenmp -O0
CPFLAGS=-lm

main: main.o benchmark.o
	$(CC) $(CFLAGS) -o benchmark main.o benchmark.o $(CPFLAGS)
