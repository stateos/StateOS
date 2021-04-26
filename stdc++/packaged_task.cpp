// example from cppreference.com
// modified by Rajmund Szymanski

#include "stm32f4_discovery.h"
#include <thread>
#include <future>
#include <functional>
#include <chrono>
 
int f(int x, int y) { return x * y; }
 
void test()
{
	std::packaged_task<int(int,int)> t1([](int a, int b) { return a * b; });
	std::future<int> r1 = t1.get_future();
	t1(5, 7);
	if (r1.get() != 35) abort();

	std::packaged_task<int()> t2(std::bind(f, 3, 11));
	std::future<int> r2 = t2.get_future();
	t2();
	if (r2.get() != 33) abort();

	std::packaged_task<int(int,int)> t3(f);
	std::future<int> r3 = t3.get_future();
	std::thread(std::move(t3), 2, 13).detach();
	if (r3.get() != 26) abort();
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
