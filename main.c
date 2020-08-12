#include "benchmark.h"
#include <stdio.h>
#include <ctype.h>
#include <stdbool.h>
#include <omp.h>

int main(int argc, char** argv)
{
	unsigned int threads = 0;
	bool FPU;
	char FPU_char;
	int score = 0;

	//Get what you want to stress
	printf("Would you like to stress floating-point (F) or integer (I)?\n");
	scanf(" %c", &FPU_char);

	//Set FPU flag based on what we're stressing
	if (tolower(FPU_char) == 'f')
	{
		FPU = true;
		printf("Running floating-point test.\n");
	}
	else if (tolower(FPU_char) == 'i')
	{
		FPU = false;
		printf("Running integer test.\n");
	}
	else
	{
		printf("Invalid input, assuming integer.\n");
		FPU = false;
	}

	//Get number of threads
	printf("Enter thread count to run test, or enter 0 to automatically run with the number of threads detected on your CPU:\n");
	scanf("%d", &threads);

	//Run the benchmark
	if (threads == 0)
		score = benchmark(FPU);
	else
		score = benchmarkThreads(threads, FPU);

	//Display the score
	printf("Score: %d\n", score);

	//Ran successfully
	return 0;
}
