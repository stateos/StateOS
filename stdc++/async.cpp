// example from cppreference.com
// modified by Rajmund Szymanski

#include "stm32f4_discovery.h"
#include <thread>
#include <future>
#include <mutex>
#include <vector>
#include <numeric>
#include <chrono>
 
struct X
{
	int foo(int i)        { return i + 3; }
	int bar(int i)        { return i + 7; }
	int operator()(int i) { return i + 11; }
};
 
template <typename It>
int work(It beg, It end)
{
	auto len = end - beg;
	if (len < 100)
		return std::accumulate(beg, end, 0);
	It mid = beg + len / 2;
	auto handle = std::async(std::launch::async, work<It>, mid, end);
	int sum = work(beg, mid);
	return sum + handle.get();
}
 
void test()
{
	std::vector<int> v(1000, 1);
	int result = work(v.begin(), v.end());
	if (result != 1000) abort();
 
	X x;
	auto a1 = std::async(&X::foo, &x, 7);
	auto a2 = std::async(std::launch::deferred, &X::bar, x, 11);
	auto a3 = std::async(std::launch::async, X(), 10);
	if (a1.get() != 10) abort();
	if (a2.get() != 18) abort();
	if (a3.get() != 21) abort();
}

int main()
{
	using namespace std::chrono_literals;
	device::Led led;
	for (;;)
	{
		test();
		std::this_thread::sleep_for(100ms);
		led.tick();
	}
}
