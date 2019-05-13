#pragma once
#include <thread>
#include <ctime>
#include <random>
#include "Math.h"
#include "calcvec.h"

namespace MT__MathLibrary
{
	class MT_Math
	{
	private:
		int iNumOfThreads;
		std::vector<std::thread> threads;
		std::vector<int64_t> retval;  // this is only temporary 
		
		 //  why is index is always 2.  Seems like it is equal to number of threads ( which is bad for an index in a zero based array)
		virtual void  BaseRealCalc(int threadID) {
			//std::cout << "BASE begin[" << index << "] is:" << begin[index] << std::endl;
			std::cout << "BASE - thread ID: " << threadID <<  std::endl;
			DoRealCalc(threadID);
		}
		virtual int DoRealCalc(int index) { return 0; };
	protected:
		std::unique_ptr<int[]> m_id;
	public:
		MT_Math(int threadNum): iNumOfThreads(threadNum) {}
		~MT_Math() {
			for (int i = 0; i < iNumOfThreads; i++) {
				//  delete any memory we've allocated
			}
		}
		void setNumofThreads(int n) {
			iNumOfThreads = n;
		}
		int getNumOfThreads() {
			return iNumOfThreads;
		}
		std::vector<int64_t> DoCalc(std::vector<int64_t> in) {
			for (int i = 0; i < iNumOfThreads; ++i){
				std::cout << "BASE - DoCalc i: " << i << std::endl;
				//std::thread threadObj(BaseRealCalc, i);  //  this doesn't compile
				//threads.emplace_back(threadObj);
				threads.emplace_back([&] {BaseRealCalc(i); });			
			}
			for (auto& t : threads) t.join();

			return retval;
		}
	};


	class MT_IsPrime : public MT_Math
	{
	private:
		std::vector<int64_t> outputs;
		virtual int DoRealCalc(int index) {
			// in here I would only look at part of the vector that was passed in.
			//  but right now my index is wrong in BASE::BaseRealCalc
			std::cout << "MT Prime: Do Real Calc: indx: " << index <<std::endl; 
			/*
			std::size_t const part_size = inputs.size() / MAX_THREADS;
			const int remainder = inputs.size() % MAX_THREADS;
			int i_start = index * part_size;
			int i_end = i_start + part_size - 1;
			if (index == MAX_THREADS - 1)
			{
				i_end += remainder;
			}
			for (int i = i_start; i <= i_end; i++) {
				outputs.at(i) = MMathLibrary::Math::IsPrime(inputs.at(i));
			}
			*/
			return 1;
		}
	protected:
	public:
		MT_IsPrime(const int threadNum) : MT_Math(threadNum) {}
		std::vector<int64_t> DoCalc(std::vector<int64_t> inVector)
		{
			outputs = inVector;  //  I am only copying the in vecoter to have something to return
			MT_Math::DoCalc(inVector);
			return outputs;
		}

	};
}