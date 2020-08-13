#include "benchmark.h"
#include <stdbool.h>
#include <omp.h>
#include <math.h>
#include <time.h>
#include <stdlib.h>
#include <limits.h>
#include <stdio.h>
#ifdef WIN32
#include <windows.h>
#else
#include <unistd.h>
#endif

#define BENCHMARK_LENGTH 30000000000

//I don't know how this works because I copied it from StackOverflow
unsigned long isqrt(unsigned long x)
{
	register unsigned long op, res, one;

	op = x;
	res = 0;

	one = 1 << 30;
	while (one > op)
		one >>= 2;

	while (one != 0)
	{
		if (op >= res + one)
		{
			op -= res + one;
			res += one << 1;
		}
		res >>= 1;
		one >>= 2;
	}

	return res;
}

//This function initializes and manages all of the threads
int benchmarkThreads(unsigned int threads, bool FPU, bool detailed)
{
	printf("Running benchmark across %d threads...\n", threads);

	Data* resData = (Data*)malloc(sizeof(Data) * threads); //Allocate array of result data based on how many threads are being spawned
	struct timespec startTime, endTime; //Create timespec structs out here since they're shared between threads

	//If we're stressing floating-point (not necessarily FPU)
	if (FPU)
		//Run in parallel with the given number of threads + 1, with the timespec structs shared across threads
		//The + 1 is for a thread that runs the timer
		#pragma omp parallel num_threads(threads) shared(startTime, endTime)
		{
			//Make the master thread initialize the timespec structs
			#pragma omp master
			clock_gettime(0, &startTime);
			clock_gettime(0, &endTime);
			#pragma omp barrier //wait for this to be done before the data is passed into the functions

			Data smallRes;
			if (omp_get_thread_num() == threads) //if we're the highest # thread (which is the + 1)
			{
				//We're the timer thread. Run this loop until 30 seconds are up
				while ((endTime.tv_nsec - startTime.tv_nsec) + (endTime.tv_sec - startTime.tv_sec) * 1000000000 < BENCHMARK_LENGTH)
				{
					//Update the time
					clock_gettime(0, &endTime);

					//Sleep for a millisecond so we don't destroy the benchmark with busy waiting
					#ifdef WIN32
					Sleep(1);
					#else
					usleep(1000);
					#endif
				}
			}
			else
			{
				smallRes = calculateFloating(startTime, &endTime); //calculate floating-point in parallel

				if (detailed)
					printf("Thread %d score: %lu\n", omp_get_thread_num(), smallRes.total);
			}

			#pragma omp critical
			resData[omp_get_thread_num()] = smallRes; //collect the data, I don't think this has to be a critical, but it can't hurt since this isn't performance-sensitive
		}
	else
		//Run in parallel with the given number of threads + 1, with the timespec structs shared across threads
		//The + 1 is for a thread that runs the timer
		#pragma omp parallel num_threads(threads + 1) shared(startTime, endTime)
		{
			//Make the master thread initialize the timespec structs
			#pragma omp master
			clock_gettime(0, &startTime);
			clock_gettime(0, &endTime);
			#pragma omp barrier //wait for this to be done before the data is passed into the functions

			Data smallRes;
			if (omp_get_thread_num() == threads) //if we're the highest # thread (which is the + 1)
			{
				//We're the timer thread. Run this loop until 30 seconds are up
				while ((endTime.tv_nsec - startTime.tv_nsec) + (endTime.tv_sec - startTime.tv_sec) * 1000000000 < BENCHMARK_LENGTH)
				{
					//Update the time
					clock_gettime(0, &endTime);

					//Sleep for a millisecond so we don't destroy the benchmark with busy waiting
					#ifdef WIN32
					Sleep(1);
					#else
					usleep(1000);
					#endif
				}
			}
			else
			{
				smallRes = calculateInteger(startTime, &endTime); //calculate integer in parallel

				if (detailed)
					printf("Thread %d score: %lu\n", omp_get_thread_num(), smallRes.total); //if the user wants a detailed result, give it to 'em!
			}

			#pragma omp critical
			resData[omp_get_thread_num()] = smallRes; //collect the data, I don't think this has to be a critical, but it can't hurt since this isn't performance-sensitive
		}

	unsigned long total = 0, in = 0;

	//collect the data from the Data structs
	for (int i = 0; i < threads; i++)
	{
		in += resData[i].in;
		total += resData[i].total;
	}

	//Free our array we malloc'd up there ^^
	free(resData);
	
	//Calculate pi
	printf("Calculated following value for pi: %lf\n", (double)in / (double)total * 4);

	//Generate the final score
	return total / 100000;
}

int benchmark(bool FPU, bool detailed)
{
	//Call the main benchmarking function with the max threads available
	return benchmarkThreads(omp_get_max_threads(), FPU, detailed);
}

//This function stresses the integer part of the CPU
Data calculateInteger(struct timespec start, struct timespec *end)
{
	//Initialize the total and in variables
	unsigned long total = 0, in = 0;
	//Create a unique seed based on the current time and the thread number
	unsigned int state = time(NULL) ^ omp_get_thread_num();

	//While the number of nanoseconds elapsed since start is less than the duration the benchmark needs to run
	while ((end->tv_nsec - start.tv_nsec) + (end->tv_sec - start.tv_sec) * 1000000000 < BENCHMARK_LENGTH)
	{
		//Generate a random short, stored in an unsigned long
		unsigned long x = rand_r(&state) % SHRT_MAX;
		unsigned long y = rand_r(&state) % SHRT_MAX;
		total++; //Increment the total

		//See if it's in; if it is, increment in
		if (isqrt(x * x + y * y) < SHRT_MAX)
			in++;
	}

	//Pack everything into our struct
	Data results;
	results.in = in;
	results.total = total;

	//And return
	return results;
}

//This function stresses the floating-point part of the CPU (could be FPU, could be SSE, etc.)
Data calculateFloating(struct timespec start, struct timespec *end)
{
	//Initialize the total and in variables
	unsigned long total = 0, in = 0;
	//Create a unique seed based on the current time and the thread number
	unsigned int state = time(NULL) ^ omp_get_thread_num();

	//While the number of nanoseconds elapsed since start is less than the duration the benchmark needs to run
	while ((end->tv_nsec - start.tv_nsec) + (end->tv_sec - start.tv_sec) * 1000000000 < BENCHMARK_LENGTH)
	{
		//Generate a random real in [0, 1]
		double x = rand_r(&state) / (double)RAND_MAX;
		double y = rand_r(&state) / (double)RAND_MAX;
		total++; //Increment the total

		//See if it's in; if it is, increment in
		if (sqrt(pow(x, 2) + pow(y, 2)) < 1)
			in++;
	}

	//Pack everything into our struct
	Data results;
	results.in = in;
	results.total = total;

	//And return
	return results;
}
