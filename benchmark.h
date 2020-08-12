#pragma once
#include <stdbool.h>
#include <time.h>

//Data struct
typedef struct
{
	unsigned long total;
	unsigned long in;
} Data;

//Calculation prototypes
Data calculateFloating(struct timespec start, struct timespec *end);
Data calculateInteger(struct timespec start, struct timespec *end);

//Benchmarking prototypes
int benchmarkThreads(unsigned int threads, bool FPU);
int benchmark(bool FPU);
