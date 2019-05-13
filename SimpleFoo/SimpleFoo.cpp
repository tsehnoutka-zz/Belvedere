#include <iostream>
#include <string>
#include <thread>
#include <stdint.h>
#include <random>
#include <iostream>
#include <chrono>
#include <functional>
#include <Windows.h>
void threadCallback(int x, std::string str)
{
	std::cout << "Passed Number = " << x << std::endl;
	std::cout << "Passed String = " << str << std::endl;
}
int main()
{
	std::vector<std::thread> threads;
	std::string str = "Sample String";
	for (int x = 0; x < 4; x++)
	{
		std::thread threadObj(threadCallback, x, str);
		threads.push_back(threadObj);
	}
	for (auto& t : threads) t.join();
	return 0;
}

/*#include <iostream>
#include <thread>
#include <vector>

class foo
{
public:
	void make_foo_func_threads()
	{
		for (int i = 0; i < 5; ++i)
			//some_threads.push_back(std::thread(&foo::foo_func,i, this));
			some_threads.emplace_back([&] {foo::foo_func( i); });
		for (auto& t : some_threads) t.join();
	}

private:
	void foo_func(int i) { std::cout << "Hello, i is : "<< i << "\n"; }
	std::vector<std::thread> some_threads;
};

int main()
{
	foo f;
	f.make_foo_func_threads();
} 
*/

