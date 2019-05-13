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
const int32_t MAX_SIZE = 10;
const int MAX_THREADS = 4;

typedef struct thread_data
{
	std::vector<int64_t> inputs;	
	std::vector<int64_t> outputs;
	int m_id;
	std::function<double(int64_t) > m_func;
	thread_data(std::vector<int64_t> in, int id, std::function<double(int64_t) > func) : inputs(in), m_id(id), m_func(func) {}

}THREAD_DATA;

DWORD WINAPI thread_func(LPVOID lpParameter)
{
	thread_data* td = (thread_data*)lpParameter;

	for (auto input : td->inputs){	
		td->outputs.push_back((int64_t)td->m_func(input));
	}
	return 0;
}
std::vector< std::vector<int64_t>> DivideVector(std::vector<int64_t> inputs)
{
	//  divide the vector one sub-vector for each thread and put them in a vector of vectors.
	std::vector< std::vector<int64_t>> inVectors;
	std::size_t const part_size = inputs.size() / MAX_THREADS;
	int remainder = inputs.size() % MAX_THREADS; // in case the bnumber of threads doesn't go into the size of inputs evenly.
	std::cout << "Remainder: " << remainder << std::endl;
	for (int i = 0; i < MAX_THREADS; i++) {
		int start = i * part_size;
		int end = start + part_size;
		if (i == MAX_THREADS - 1)
			end += remainder;
		std::cout << "Start: " << start << "\t end: " << end << "\tdiff: " << end - start << "\tremainder: " << remainder <<std::endl;
		std::vector<int64_t> split(inputs.begin() + start, inputs.begin() + end);
		std::cout << "split size is: " << split.size() << std::endl;
		inVectors.push_back(split);
	}
	return inVectors;
}

static std::vector<int64_t> DoCalc(const std::vector<int64_t>& inputs)
{
	std::vector<int64_t> outputs;
	outputs.reserve(inputs.size());
	HANDLE  hThreadArray[MAX_THREADS];
	THREAD_DATA* myThreadData[MAX_THREADS];
	std::vector< std::vector<int64_t>> inVectors = DivideVector( inputs);
	
	for (int i = 0; i < MAX_THREADS; i++){
		myThreadData[i] = new thread_data(inVectors.at(i), i, MathLibrary::Math::IsEven);
		hThreadArray[i] =CreateThread(NULL, 0, thread_func, myThreadData[i], 0, 0);
		if (hThreadArray[i] == NULL)
		{
			std::cout <<  "Error created thread" << std::endl;
			ExitProcess(3);
		}
	}
	//  wait for all the thread to finish
	WaitForMultipleObjects(MAX_THREADS, hThreadArray, TRUE, INFINITE);

	// combine all the threads output vecors into one vector
	for (int i = 0; i < MAX_THREADS; i++){
		outputs.insert(outputs.end(),myThreadData[i]->outputs.begin(), myThreadData[i]->outputs.end());
		delete myThreadData[i];
	}
	return outputs;
}

int main()
{
	std::default_random_engine generator;  //  the built-in random number generator
	std::uniform_int_distribution<int64_t> distribution(1, MAX_NUM);   //  defines the random between the specified range
	std::vector<int64_t> inputs;  //  array to hold the random numbers

	//  create vector of random numbers
	for (int32_t i = 0; i < MAX_SIZE; ++i){
		inputs.push_back(distribution(generator));
	}


	auto tick = std::chrono::high_resolution_clock::now();
	std::vector<int64_t> outputs = DoCalc(inputs);
	auto tock = std::chrono::high_resolution_clock::now();
	std::cout << "finished: " << (std::chrono::duration_cast<std::chrono::duration<double>>(tock - tick)).count() << std::endl;

	for (int32_t i = 0; i < MAX_SIZE; ++i) {
		std::cout << i << "\t" << inputs.at(i) << "\t" << outputs.at(i) << std::endl;
	}
}

