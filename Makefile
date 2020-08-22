CC=gcc
CFLAGS=-fopenmp -O2 -std=c11 -pedantic-errors -Werror -Wall -D _BSD_SOURCE
CPFLAGS=-lm

main: main.o benchmark.o
	$(CC) $(CFLAGS) -o benchmark main.o benchmark.o $(CPFLAGS)
