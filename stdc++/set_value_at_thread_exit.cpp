// example from cppreference.com
// modified by Rajmund Szymanski

#include "stm32f4_discovery.h"
#include <thread>
#include <future>
#include <chrono>
 
void test()
{
	using namespace std::chrono_literals;
	std::promise<int> p;
	std::future<int> f = p.get_future();
	std::thread([&p]
	{
		std::this_thread::sleep_for(100ms);
		p.set_value_at_thread_exit(11);
	}).detach();
	f.wait();
	if (f.get() != 11) abort();
}

int main()
{
	device::Led led;
	for (;;)
	{
		test();
		led.tick();
	}
}
