// example from cppreference.com
// modified by Rajmund Szymanski

#include "stm32f4_discovery.h"
#include <thread>
#include <vector>
#include <chrono>

device::Led led;

void test()
{
	const unsigned nums[] = { 0, 1, 2, 3 };
 
	std::barrier sync_point(std::size(nums), [&nums]{
		for (auto const num : nums) {
			led.toggle(num);
		}
	});
 
	auto work = [&sync_point](unsigned num) {
		sync_point.arrive_and_wait();
		led.toggle(num);
		sync_point.arrive_and_wait();
		led.toggle(num);
	};
 
	std::vector<std::thread> threads;
	for (auto const num : nums) {
		threads.emplace_back(work, num);
	}
	for (auto& thread : threads) {
		thread.join();
	}
}

int main()
{
	for (;;)
	{
		std::this_thread::sleep_for(std::chrono::milliseconds{100});
		test();
		led.tick();
	}
}
