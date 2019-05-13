// temp.cpp : This file contains the 'main' function. Program execution begins and ends there.
//

#include <stdint.h>
#include <random>
#include <iostream>
#include <chrono>
#include <functional>
#include <Windows.h>
#include "MT_Math.h"

const int64_t MAX_NUM = 100000000000;
const int32_t MAX_SIZE = 500000;
const int MAX_THREADS = 2;

int main()
{
	std::default_random_engine generator;  //  the built-in random number generator
	std::uniform_int_distribution<int64_t> distribution(1, MAX_NUM);   //  defines the random between the specified range
	std::vector<int64_t> inputs;  //  array to hold the randomnumbers
	MT__MathLibrary::MT_IsPrime mtPrime(MAX_THREADS);

	//  create array of random numbers
	for (int32_t i = 0; i < MAX_SIZE; ++i)
	{
		inputs.push_back(distribution(generator));
	}

	auto tick = std::chrono::high_resolution_clock::now();
	//mtPrime.setNumofThreads(MAX_THREADS);
	std::vector<int64_t> outputs = mtPrime.DoCalc(inputs);
	auto tock = std::chrono::high_resolution_clock::now();
	std::cout << "finished: " << (std::chrono::duration_cast<std::chrono::duration<double>>(tock - tick)).count() << std::endl;
}
