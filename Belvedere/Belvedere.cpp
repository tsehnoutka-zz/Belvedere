#include <stdint.h>
#include <random>
#include <iostream>
#include <chrono>
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

static std::vector<int64_t> DoCalc(const std::vector<int64_t>& inputs)
{
	std::vector<int64_t> outputs;
	outputs.reserve(inputs.size());
	for (auto input : inputs)
	{
		outputs.push_back(MathLibrary::Math::IsPrime(input));
	}

	return outputs;
}

int main(int argc, char* argv[])
{
	std::default_random_engine generator;
	std::uniform_int_distribution<int64_t> distribution(1, 1000000000000);
	std::vector<int64_t> inputs;
	for (int32_t i = 0; i < 500000; ++i)
	{
		inputs.push_back(distribution(generator));
	}
	auto tick = std::chrono::high_resolution_clock::now();
	std::vector<int64_t> outputs = DoCalc(inputs);
	auto tock = std::chrono::high_resolution_clock::now();
	std::cout << "finished: " << (std::chrono::duration_cast<std::chrono::duration<double>>(tock - tick)).count() << std::endl;
	return 0;
};
