// example from cppreference.com
// modified by Rajmund Szymanski

#include "stm32f4_discovery.h"
#include <thread>
#include <future>
#include <chrono>
 
void test()
{   
	std::promise<void> p0, p1, p2;
	std::shared_future<void> ready(p0.get_future());

	auto fun1 = [&, ready]() {
		p1.set_value();
		ready.wait();
		return 7;
	};

	auto fun2 = [&, ready]() {
		p2.set_value();
		ready.wait();
		return 5;
	};
 
	auto f1 = p1.get_future();
	auto f2 = p2.get_future();
 
	auto result1 = std::async(std::launch::async, fun1);
	auto result2 = std::async(std::launch::async, fun2);
 
	f1.wait();
	f2.wait();

	p0.set_value();
 
	if (result1.get() != 7) abort();
	if (result2.get() != 5) abort();
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
