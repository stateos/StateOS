// example from cppreference.com
// modified by Rajmund Szymanski

#include "stm32f4_discovery.h"
#include <thread>
#include <future>
#include <chrono>
 
void test()
{
	std::packaged_task<int()> task([]{ return 11; });
	std::future<int> f1 = task.get_future();
	std::jthread t(std::move(task));

	std::future<int> f2 = std::async(std::launch::async, []{ return 13; });
 
	std::promise<int> p;
	std::future<int> f3 = p.get_future();
	std::thread( [&p]{ p.set_value_at_thread_exit(17); }).detach();
 
	f1.wait();
	f2.wait();
	f3.wait();

	if (f1.get() != 11) abort();
	if (f2.get() != 13) abort();
	if (f3.get() != 17) abort();
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
