#include <stdint.h>
#include <random>
#include <iostream>
#include <chrono>
#include <functional>
#include <Windows.h>
#include "Math.h"

/* Our trading system contains a number of calculation engines that perform calcs on a request basis.
 * The goal of this assignment is to come up with a generic framework for accelerating the calculations
 * in these engines. We have given you a benchmark test that provides a workload that is representative
 * of the types of calculations we are trying to accelerate.
 *
 * The goal of this assignment is to get the provided test to run as quickly as possible.
 *
 * Your solution should be generic. It should be easily extendible to methods other than IsPrime. It should
 * also be extendible to calculations with various function signatures. You should not modify IsPrime or Math.h.
*/

/*  
 * This only works for additional functions that have the declaration:
 * double foo(int64_t n)
 * If this was to go on other OSs besides Windows, I would create a class thathandled thte thread work.
 * It would determine whcih OS we were on, and call those OS specific functions.
*/
const int64_t MAX_NUM = 100000000000;
const int32_t MAX_SIZE = 500000;
const int MAX_THREADS = 2;

typedef struct Primethread_data
{
	const std::vector<int64_t> inputs;
	std::vector<int64_t> outputs;
	int begin;
	int end;
	int m_id;
	std::function<double(int64_t) > m_func;
	Primethread_data(const std::vector<int64_t>& in, std::vector<int64_t>& out, int b, int e, int id, std::function<double(int64_t) > func) : inputs(in), outputs(out), begin(b), end(e), m_id(id), m_func(func) {}

}PRIME_THREAD_DATA;

typedef struct Sumthread_data
{
	const std::vector<double> inputs;
	double dSum;
	int begin;
	int end;
	int m_id;
	std::function<double(double) > m_func;
	Sumthread_data(const std::vector<double>& in, int b, int e, int id, std::function<double(double) > func ) : inputs(in), begin(b), end(e), m_id(id), m_func(func), dSum(0.0) {}
		

}SUM_THREAD_DATA;

DWORD WINAPI Primethread_func(LPVOID lpParameter)
{
	Primethread_data* td = (Primethread_data*)lpParameter;

	for (int i = td->begin; i <= td->end; i++) {
		td->outputs.at(i) = (int64_t)td->m_func(td->inputs.at(i));
	}
	return 0;
}
DWORD WINAPI Sumthread_func(LPVOID lpParameter)
{
	Sumthread_data* td = (Sumthread_data*)lpParameter;

	for (int i = td->begin; i <= td->end; i++) {
		td->dSum = td->m_func(td->inputs.at(i));
	}
	return 0;
}

static std::vector<int64_t> DoIsPrime(const std::vector<int64_t>& inputs)
{
	std::vector<int64_t> outputs;
	outputs.resize(inputs.size());
	int i_start = 0;
	int i_end = 0;
	int i = 0;
	HANDLE  hThreadArray[MAX_THREADS];
	PRIME_THREAD_DATA* myThreadData[MAX_THREADS];
	std::size_t const part_size = inputs.size() / MAX_THREADS;
	const int remainder = inputs.size() % MAX_THREADS;

	//  create all the threads
	for (i = 0; i < MAX_THREADS; i++) 
	{
		i_start = i * part_size;
		i_end = i_start + part_size - 1;
		if (i == MAX_THREADS - 1) 
		{
			i_end += remainder;
		}
		myThreadData[i] = new Primethread_data(inputs, outputs, i_start, i_end, i, MathLibrary::Math::IsPrime);
		hThreadArray[i] = CreateThread(NULL, 0, Primethread_func, myThreadData[i], 0, 0);

		if (hThreadArray[i] == NULL)
		{
			std::cout << "Error created thread" << std::endl;
			ExitProcess(3);
		}
	}
	//  wait for all the thread to finish
	WaitForMultipleObjects(MAX_THREADS, hThreadArray, TRUE, INFINITE);

	// delete the memory or the thread data structures
	for (i = 0; i < MAX_THREADS; i++) 
	{
		delete myThreadData[i];
	}

	return outputs;
}
double DoSum(const std::vector<double>& inputs)
{
	double dSum = 0.0;
	int i_start = 0;
	int i_end = 0;
	int i = 0;
	HANDLE  hThreadArray[MAX_THREADS];
	SUM_THREAD_DATA* myThreadData[MAX_THREADS];
	std::size_t const part_size = inputs.size() / MAX_THREADS;
	const int remainder = inputs.size() % MAX_THREADS;

	//  create all the threads
	for (i = 0; i < MAX_THREADS; i++)
	{
		i_start = i * part_size;
		i_end = i_start + part_size - 1;
		if (i == MAX_THREADS - 1)
		{
			i_end += remainder;
		}
		myThreadData[i] = new Sumthread_data(inputs, i_start, i_end, i, MathLibrary::Math::Sum);
		hThreadArray[i] = CreateThread(NULL, 0, Sumthread_func, myThreadData[i], 0, 0);

		if (hThreadArray[i] == NULL)
		{
			std::cout << "Error created thread" << std::endl;
			ExitProcess(3);
		}
	}
	//  wait for all the thread to finish
	WaitForMultipleObjects(MAX_THREADS, hThreadArray, TRUE, INFINITE);

	// delete the memory or the thread data structures
	for (i = 0; i < MAX_THREADS; i++)
	{
		dSum += myThreadData[i]->dSum;
		delete myThreadData[i];
	}

	return dSum;
}


int main()
{
	std::default_random_engine generator;  //  the built-in random number generator
	std::uniform_int_distribution<int64_t> distribution(1, MAX_NUM);   //  defines the random between the specified range
	std::vector<int64_t> inputs;  //  array to hold the random numbers
	std::vector<double> dInputs(inputs.begin(), inputs.end());


	//  create vector of random numbers
	for (int32_t i = 0; i < MAX_SIZE; ++i) {
		inputs.push_back(distribution(generator));
	}

	auto tick = std::chrono::high_resolution_clock::now();
	std::vector<int64_t> outputs = DoIsPrime(inputs);
	double mySum = DoSum(dInputs);
	auto tock = std::chrono::high_resolution_clock::now();
	std::cout << "finished: " << (std::chrono::duration_cast<std::chrono::duration<double>>(tock - tick)).count() << std::endl;

	std::cout << "The Sum is : " << mySum << std::endl;
	return 0;
}