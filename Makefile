CC=gcc
CFLAGS=-fopenmp -O2 -pedantic-errors -Werror -Wall
CPFLAGS=-lm

main: main.o benchmark.o
	$(CC) $(CFLAGS) -o benchmark main.o benchmark.o $(CPFLAGS)
