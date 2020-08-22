#define _POSIX_C_SOURCE 199309L
#include "benchmark.h"
#include <stdio.h>
#include <ctype.h>
#include <stdbool.h>
#include <omp.h>
#include <string.h>

int main(int argc, char** argv)
{
	unsigned int threads = 0;
	bool FPU, detailed = false;
	char FPU_char;
	int score = 0;
	
	//See if they want a detailed score
	if (argc == 2 && !strcmp(argv[1], "-d"))
		detailed = true;
	
	//See if they help
	if (argc == 2 && !strcmp(argv[1], "-h"))
	{
		printf("There is only one flag, and it is -d, which outputs a more detailed score. The rest is self-explanatory.\n");
		return 0;
	}

	#pragma GCC diagnostic push
	#pragma GCC diagnostic ignored "-Wunused-result"

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
	scanf("%ud", &threads);
	
	#pragma GCC diagnostic pop

	//Run the benchmark
	if (threads == 0)
		score = benchmark(FPU, detailed);
	else
		score = benchmarkThreads(threads, FPU, detailed);

	//Display the score
	printf("Score: %d\n", score);

	//Ran successfully
	return 0;
}
