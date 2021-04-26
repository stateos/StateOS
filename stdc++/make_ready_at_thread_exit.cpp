// example from cppreference.com
// modified by Rajmund Szymanski

#include "stm32f4_discovery.h"
#include <thread>
#include <future>
#include <chrono>
 
void work(std::future<void>& output)
{
	std::packaged_task<void(bool&)> tsk{ [](bool& done) { done = true; } };
	auto result = tsk.get_future();
	bool done = false;
	tsk.make_ready_at_thread_exit(done);
	if (!done)
		return;
	auto status = result.wait_for(std::chrono::seconds(0));
	if (status != std::future_status::timeout)
		return;
	output = std::move(result);
}
 
void test()
{
	using namespace std::chrono_literals;
	std::future<void> result;
	std::thread{ work, std::ref(result) }.join();
	auto status = result.wait_for(0s);
	if (status != std::future_status::ready) abort();
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
